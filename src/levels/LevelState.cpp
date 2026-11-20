#include "levels/LevelState.hpp"
#include "Game.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

LevelState::LevelState(
    sf::RenderWindow* window,
    Game* game,
    Designar::DynArray<LevelEncounter> encounters,
    LevelWorldBounds worldBounds,
    float cameraFollowSpeed)
    : State(window, game),
      encounters(std::move(encounters)),
      encounterStatuses(
          static_cast<int>(this->encounters.size()),
          EncounterStatus::Pending),
      worldBounds(worldBounds),
      cameraView(window->getDefaultView()),
      cameraFollowSpeed(std::max(0.f, cameraFollowSpeed))
{
}

void LevelState::update(float deltaTime)
{
    updateLevel(deltaTime, isPlayerMovementBlocked());
    processEncounterTriggers();
    updateCamera(deltaTime);
}

void LevelState::handleEvent(sf::Event& event)
{
    handleLevelEvent(event);
}

void LevelState::draw()
{
    window->setView(cameraView);
    drawWorld();
    window->setView(window->getDefaultView());
    drawHud();
}

void LevelState::completeEncounter(int encounterIndex)
{
    if (encounterIndex < 0 ||
        encounterIndex >= static_cast<int>(encounterStatuses.size()) ||
        encounterStatuses[encounterIndex] != EncounterStatus::Active)
    {
        return;
    }

    encounterStatuses[encounterIndex] = EncounterStatus::Completed;
}

bool LevelState::completeLevelProgress()
{
    if (levelCompleted || game == nullptr || !game->completeCurrentLevel())
    {
        return false;
    }

    levelCompleted = true;
    return true;
}

bool LevelState::isPlayerMovementBlocked() const
{
    for (int index = 0; index < static_cast<int>(encounters.size()); ++index)
    {
        if (encounters[index].type == LevelEncounter::Type::Wave &&
            encounterStatuses[index] == EncounterStatus::Active)
        {
            return true;
        }
    }

    return false;
}

const sf::View& LevelState::getCameraView() const
{
    return cameraView;
}

void LevelState::processEncounterTriggers()
{
    for (int index = 0; index < static_cast<int>(encounters.size()); ++index)
    {
        if (encounterStatuses[index] != EncounterStatus::Pending ||
            !canActivateEncounter(index))
        {
            continue;
        }

        encounterStatuses[index] = EncounterStatus::Active;
        spawnEncounter(index, encounters[index]);
    }
}

bool LevelState::canActivateEncounter(int encounterIndex) const
{
    const LevelEncounter& encounter = encounters[encounterIndex];
    if (encounter.trigger == LevelEncounter::Trigger::AtPosition)
    {
        return getPlayerPosition().x >= encounter.triggerX;
    }

    return encounterIndex > 0 &&
           encounterStatuses[encounterIndex - 1] == EncounterStatus::Completed;
}

void LevelState::updateCamera(float deltaTime)
{
    sf::Vector2f center = cameraView.getCenter();
    const sf::Vector2f target = getPlayerPosition();
    const float interpolation = 1.f - std::exp(-cameraFollowSpeed * deltaTime);

    center.x += (target.x - center.x) * interpolation;
    center.y = target.y;
    clampCameraCenter(center);
    cameraView.setCenter(center);
}

void LevelState::clampCameraCenter(sf::Vector2f& center) const
{
    const sf::Vector2f halfView = cameraView.getSize() / 2.f;

    if (worldBounds.maximum.x - worldBounds.minimum.x <= cameraView.getSize().x)
    {
        center.x = (worldBounds.minimum.x + worldBounds.maximum.x) / 2.f;
    }
    else
    {
        center.x = std::clamp(
            center.x,
            worldBounds.minimum.x + halfView.x,
            worldBounds.maximum.x - halfView.x);
    }

    if (worldBounds.maximum.y - worldBounds.minimum.y <= cameraView.getSize().y)
    {
        center.y = (worldBounds.minimum.y + worldBounds.maximum.y) / 2.f;
    }
    else
    {
        center.y = std::clamp(
            center.y,
            worldBounds.minimum.y + halfView.y,
            worldBounds.maximum.y - halfView.y);
    }
}
