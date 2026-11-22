
#include "levels/Level2.hpp"
#include "Soldier.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace
{

// El escenario tendrá tres pantallas de ancho.
LevelWorldBounds getLevel2WorldBounds(const sf::RenderWindow* window)
{
    const sf::Vector2u windowSize = window->getSize();

    const float worldWidth =
        static_cast<float>(windowSize.x) * 3.f;

    const float worldHeight =
        static_cast<float>(windowSize.y);

    return {
        {0.f, 0.f},
        {worldWidth, worldHeight}
    };
}

// Enemigos del nivel 2.
Designar::DynArray<LevelEncounter> createLevel2Encounters(
    const sf::RenderWindow* window)
{
    const sf::Vector2u windowSize = window->getSize();

    const float worldWidth =
        static_cast<float>(windowSize.x) * 3.f;

    LevelEncounter encounter{
        LevelEncounter::Type::Wave,
        LevelEncounter::Trigger::AtPosition,
        static_cast<float>(windowSize.x) * 1.2f,
        Designar::DynArray<LevelEnemySpawn>()
    };

    encounter.enemies.append({
        "cientifico1",
        {worldWidth * 0.55f, static_cast<float>(windowSize.y) * 0.82f}
    });

    encounter.enemies.append({
        "cientifico1",
        {worldWidth * 0.65f, static_cast<float>(windowSize.y) * 0.82f}
    });

    Designar::DynArray<LevelEncounter> encounters;
    encounters.append(std::move(encounter));

    return encounters;
}

}

Level2::Level2(sf::RenderWindow* window, Game* game)
    : LevelState(
        window,
        game,
        "assets/fondo_pandemia_dia.png",
        createLevel2Encounters(window),
        getLevel2WorldBounds(window))
{
    const sf::Vector2u windowSize = window->getSize();

    const float worldWidth =
        static_cast<float>(windowSize.x) * 3.f;

    worldBounds = sf::FloatRect(
        {0.f, 0.f},
        {
            worldWidth,
            static_cast<float>(windowSize.y)
        });

    player.setPosition({
        static_cast<float>(windowSize.x) * 0.15f,
        static_cast<float>(windowSize.y) * 0.82f
    });

    player.setVisualScale(2.5f);
    player.setCombatStats(35, 130.f, 0.45f);
}

sf::Vector2f Level2::getPlayerPosition() const
{
    return player.getPosition();
}

void Level2::handleLevelEvent(sf::Event& event)
{
    // Aquí se pueden agregar los eventos del nivel 2.
}

void Level2::updateLevel(
    float deltaTime,
    bool playerMovementBlocked,
    std::optional<LevelArenaBounds>)
{
    const sf::Vector2f previousPlayerPosition =
        player.getPosition();

    player.update(deltaTime, !playerDefeated && !playerMovementBlocked);

    resolveMovement(player, previousPlayerPosition);

    if (player.isDead())
    {
        playerDefeated = true;
    }

    for (std::size_t index = 0; index < enemies.size();)
    {
        Enemy& enemy = *enemies[index];

        const sf::Vector2f previousPosition =
            enemy.getPosition();

        enemy.updateAI(deltaTime, player);

        resolveMovement(enemy, previousPosition);

        if (enemy.isDead())
        {
            enemies.remove_pos_closing_breach(index);
        }
        else
        {
            ++index;
        }
    }

    if (activeEncounterIndex >= 0 && enemies.is_empty())
    {
        completeEncounter(activeEncounterIndex);
        activeEncounterIndex = -1;

        if (areAllEncountersCompleted() && !levelCompleted)
        {
            levelCompleted = completeLevelProgress();
        }
    }
}

void Level2::drawWorld()
{
    player.draw(*window);

    for (std::size_t index = 0; index < enemies.size(); ++index)
    {
        enemies[index]->draw(*window);
    }
}

void Level2::drawHud()
{
    const sf::Vector2u windowSize = window->getSize();

    sf::RectangleShape healthBackground({220.f, 18.f});
    healthBackground.setPosition({20.f, 20.f});
    healthBackground.setFillColor(sf::Color(30, 30, 30, 220));
    window->draw(healthBackground);

    const float healthFraction =
        player.getMaxHp() > 0
        ? std::clamp(
            static_cast<float>(player.getHp()) /
            static_cast<float>(player.getMaxHp()),
            0.f,
            1.f)
        : 0.f;

    sf::RectangleShape healthFill({
        220.f * healthFraction,
        18.f
    });

    healthFill.setPosition({20.f, 20.f});
    healthFill.setFillColor(sf::Color(55, 190, 85));
    window->draw(healthFill);
}

void Level2::spawnEncounter(
    int encounterIndex,
    const LevelEncounter& encounter)
{
    if (activeEncounterIndex >= 0 || !enemies.is_empty())
    {
        throw std::logic_error(
            "No se puede iniciar otra oleada mientras haya enemigos activos");
    }

    activeEncounterIndex = encounterIndex;

    for (std::size_t index = 0;
         index < encounter.enemies.size();
         ++index)
    {
        const std::string& enemyType =
            encounter.enemies[index].enemyType;

        Designar::DynArray<std::unique_ptr<Enemy>> createdEnemies =
            enemyFactory.createEnemies(enemyType, 1);

        for (std::size_t enemyIndex = 0;
             enemyIndex < createdEnemies.size();
             ++enemyIndex)
        {
            std::unique_ptr<Enemy>& enemy =
                createdEnemies[enemyIndex];

            enemy->setPosition(
                encounter.enemies[index].position);

            enemy->setVisualScale(2.5f);

            enemies.append(std::move(enemy));
        }
    }
}

void Level2::initializeCollisionAreas()
{
    collisionAreas.clear();
}

bool Level2::collidesAt(const sf::Vector2f& position) const
{
    constexpr float bodyHalfWidth = 28.f;
    constexpr float bodyHeight = 36.f;

    if (position.x < bodyHalfWidth ||
        position.x > worldBounds.position.x + worldBounds.size.x - bodyHalfWidth ||
        position.y < bodyHeight ||
        position.y > worldBounds.size.y)
    {
        return true;
    }

    const sf::FloatRect body(
        {position.x - bodyHalfWidth, position.y - bodyHeight},
        {bodyHalfWidth * 2.f, bodyHeight});

    for (const sf::FloatRect& area : collisionAreas)
    {
        if (area.findIntersection(body))
        {
            return true;
        }
    }

    return false;
}

void Level2::resolveMovement(
    Entity& entity,
    const sf::Vector2f& previousPosition)
{
    constexpr float bodyHalfWidth = 28.f;
    constexpr float bodyHeight = 36.f;

    const sf::Vector2f target = entity.getPosition();

    sf::Vector2f resolved = previousPosition;

    sf::Vector2f candidate(
        std::clamp(
            target.x,
            bodyHalfWidth,
            worldBounds.size.x - bodyHalfWidth),
        resolved.y);

    if (!collidesAt(candidate))
    {
        resolved.x = candidate.x;
    }

    candidate = {
        resolved.x,
        std::clamp(
            target.y,
            bodyHeight,
            worldBounds.size.y)
    };

    if (!collidesAt(candidate))
    {
        resolved.y = candidate.y;
    }

    entity.setPosition(resolved);
}
