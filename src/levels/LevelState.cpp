#include "levels/LevelState.hpp"
#include "Game.hpp"
#include "States/OptionsState.hpp"
#include "Player.hpp"
#include "item/InventoryHud.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

LevelState::LevelState(
    sf::RenderWindow* window,
    Game* game,
    const std::string& backgroundPath,
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
      cameraFollowSpeed(std::max(0.f, cameraFollowSpeed)),
      backgroundSprite(backgroundTexture)
{
    if (!backgroundTexture.loadFromFile(backgroundPath))
    {
        throw std::runtime_error("No se pudo cargar el fondo del nivel: " + backgroundPath);
    }

    backgroundTexture.setSmooth(false);
    const sf::Vector2u textureSize = backgroundTexture.getSize();
    backgroundSprite.setTextureRect(
        sf::IntRect(
            {0, 0},
            {static_cast<int>(textureSize.x), static_cast<int>(textureSize.y)}));

    const sf::Vector2f worldSize(
        worldBounds.maximum.x - worldBounds.minimum.x,
        worldBounds.maximum.y - worldBounds.minimum.y);
    backgroundSprite.setPosition(worldBounds.minimum);
    backgroundSprite.setScale({
        worldSize.x / static_cast<float>(textureSize.x),
        worldSize.y / static_cast<float>(textureSize.y)
    });
}

void LevelState::update(float deltaTime)
{
    updateLevel(
        deltaTime,
        isPlayerMovementBlocked(),
        getActiveArenaBounds());
    processEncounterTriggers();
    updateCamera(deltaTime);
}

void LevelState::handleEvent(sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (keyPressed != nullptr)
    {
        if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            game->pushState(
                std::make_unique<OptionsState>(window, game, *this));
            return;
        }

        if (getPlayer().selectInventorySlotFromKey(keyPressed->code))
        {
            return;
        }
    }

    handleLevelEvent(event);
}

void LevelState::draw()
{
    window->setView(cameraView);
    window->draw(backgroundSprite);
    drawWorld();
    window->setView(window->getDefaultView());
    drawHud();
    const Player& player = getPlayer();
    drawInventoryHud(
        *window,
        player.getInventory(),
        player.getSelectedInventorySlot());
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

bool LevelState::areAllEncountersCompleted() const
{
    for (std::size_t index = 0; index < encounterStatuses.size(); ++index)
    {
        if (encounterStatuses[index] != EncounterStatus::Completed)
        {
            return false;
        }
    }

    return true;
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
    for (std::size_t index = 0; index < encounters.size(); ++index)
    {
        if (encounters[index].type == LevelEncounter::Type::Wave &&
            encounterStatuses[index] == EncounterStatus::Active)
        {
            return true;
        }
    }

    return false;
}

std::optional<LevelArenaBounds> LevelState::getActiveArenaBounds() const
{
    for (std::size_t index = 0; index < encounters.size(); ++index)
    {
        if (encounterStatuses[index] == EncounterStatus::Active &&
            encounters[index].arenaBounds.has_value())
        {
            return encounters[index].arenaBounds;
        }
    }

    return std::nullopt;
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
    if (encounter.type == LevelEncounter::Type::Wave)
    {
        for (std::size_t index = 0; index < encounterStatuses.size(); ++index)
        {
            if (encounterStatuses[index] == EncounterStatus::Active &&
                encounters[index].type == LevelEncounter::Type::Wave)
            {
                return false;
            }
        }
    }

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
