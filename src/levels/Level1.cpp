#include "levels/Level1.hpp"
#include "Soldier.hpp"
#include "item/Weapon.hpp"
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace
{
LevelWorldBounds getWorldBounds(const sf::RenderWindow* window)
{
    const sf::Vector2u size = window->getSize();
    return {
        {0.f, 0.f},
        {static_cast<float>(size.x), static_cast<float>(size.y)}
    };
}

Designar::DynArray<LevelEncounter> createLevel1Encounters(
    const sf::RenderWindow* window)
{
    const sf::Vector2u windowSize = window->getSize();
    LevelEncounter wave{
        LevelEncounter::Type::Wave,
        LevelEncounter::Trigger::AtPosition,
        0.f,
        Designar::DynArray<LevelEnemySpawn>()
    };

    constexpr std::size_t soldierCount = 2;
    for (std::size_t index = 0; index < soldierCount; ++index)
    {
        const float horizontalPosition =
            0.52f + static_cast<float>(index) * 0.08f;
        wave.enemies.append({
            "soldier",
            {
                static_cast<float>(windowSize.x) * horizontalPosition,
                static_cast<float>(windowSize.y) * 0.82f
            }
        });
    }

    Designar::DynArray<LevelEncounter> encounters;
    encounters.append(std::move(wave));
    return encounters;
}
}

Level1::Level1(sf::RenderWindow* window, Game* game)
    : LevelState(
          window,
          game,
          "assets/2guerra.png",
          createLevel1Encounters(window),
          getWorldBounds(window))
{
    const sf::Vector2u windowSize = window->getSize();

    initializeCollisionAreas();
    worldBounds = sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    player.setPosition({
        static_cast<float>(windowSize.x) * 0.15f,
        static_cast<float>(windowSize.y) * 0.82f
    });
    player.setVisualScale(2.5f);
    player.setCombatStats(35, 130.f, 0.45f);

    worldWeapon = std::make_unique<Weapon>();
    worldWeapon->setPosition({
        player.getPosition().x + 60.f,
        player.getPosition().y - 8.f
    });
}

sf::Vector2f Level1::getPlayerPosition() const
{
    return player.getPosition();
}

void Level1::handleLevelEvent(sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (keyPressed == nullptr ||
        keyPressed->code != sf::Keyboard::Key::E ||
        worldWeapon == nullptr ||
        !player.getInventory().hasSpace() ||
        playerDefeated)
    {
        return;
    }

    worldWeapon->tryPickup(player.getPosition(), 80.f);
}

void Level1::updateLevel(float deltaTime, bool)
{
    const sf::Vector2f previousPlayerPosition = player.getPosition();
    player.update(deltaTime, !playerDefeated);
    resolveMovement(player, previousPlayerPosition);

    if (player.isDead())
    {
        playerDefeated = true;
    }

    updateWorldWeapon(deltaTime);
    updatePlayerWeapon(deltaTime);

    for (std::size_t index = 0; index < enemies.size();)
    {
        Enemy& enemy = *enemies[index];
        const sf::Vector2f previousPosition = enemy.getPosition();
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

    updatePlayerBullets(deltaTime);

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

void Level1::drawWorld()
{
    if (worldWeapon != nullptr)
    {
        worldWeapon->draw(*window);
    }

    player.draw(*window);
    if (Weapon* weapon = player.getEquippedWeapon())
    {
        const sf::Vector2f playerPosition = player.getPosition();
        const float facingOffset = player.isFacingRight() ? 18.f : -18.f;
        weapon->setPosition({
            playerPosition.x + facingOffset,
            playerPosition.y - 58.f
        });
        weapon->getAnimation().setFacingRight(player.isFacingRight());
        weapon->drawHeld(*window);
    }

    for (std::size_t index = 0; index < enemies.size(); ++index)
    {
        enemies[index]->draw(*window);
    }

    for (const Bullet& bullet : playerBullets)
    {
        bullet.draw(*window);
    }
}

void Level1::drawHud()
{
    const sf::Vector2u windowSize = window->getSize();
    const auto drawHealthBar = [this](const sf::Vector2f& position, int hp, int maxHp,
                                      const sf::Color& color)
    {
        const sf::Vector2f size(220.f, 18.f);
        sf::RectangleShape background(size);
        background.setPosition(position);
        background.setFillColor(sf::Color(30, 30, 30, 220));
        window->draw(background);

        const float fraction = maxHp > 0
            ? std::clamp(static_cast<float>(hp) / static_cast<float>(maxHp), 0.f, 1.f)
            : 0.f;
        sf::RectangleShape fill({size.x * fraction, size.y});
        fill.setPosition(position);
        fill.setFillColor(color);
        window->draw(fill);
    };

    drawHealthBar({20.f, 20.f}, player.getHp(), player.getMaxHp(),
                  sf::Color(55, 190, 85));
    if (!enemies.is_empty())
    {
        drawHealthBar(
            {static_cast<float>(windowSize.x) - 240.f, 20.f},
            enemies[0]->getHp(),
            enemies[0]->getMaxHp(),
            sf::Color(200, 55, 45));
    }

    if (const Weapon* weapon = player.getEquippedWeapon())
    {
        drawAmmoCounter(weapon->getAmmo());
    }
}

void Level1::updateWorldWeapon(float deltaTime)
{
    if (worldWeapon == nullptr)
    {
        return;
    }

    worldWeapon->update(deltaTime, player.getPosition());
    if (worldWeapon->isCollected() &&
        !player.addItem(std::move(worldWeapon)))
    {
        throw std::logic_error("No se pudo transferir la MP40 al inventario");
    }
}

void Level1::updatePlayerWeapon(float deltaTime)
{
    Weapon* weapon = player.getEquippedWeapon();
    if (weapon == nullptr)
    {
        return;
    }

    weapon->updateFireCooldown(deltaTime);
    if (playerDefeated ||
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
        !weapon->tryFire())
    {
        return;
    }

    const sf::Vector2f playerPosition = player.getPosition();
    const float direction = player.isFacingRight() ? 1.f : -1.f;
    playerBullets.append(Bullet(
        sf::Vector2f(
            playerPosition.x + direction * 38.f,
            playerPosition.y - 74.f),
        direction,
        700.f,
        static_cast<int>(weapon->getDamage()),
        sf::Color(250, 210, 80)));

    if (weapon->getAmmo() == 0)
    {
        Inventory& inventory = player.getInventory();
        for (std::size_t index = 0; index < inventory.capacity(); ++index)
        {
            if (inventory.get(index) == weapon)
            {
                std::unique_ptr<Item> depletedWeapon = inventory.removeAt(index);
                break;
            }
        }
    }
}

void Level1::updatePlayerBullets(float deltaTime)
{
    Designar::DynArray<Entity*> targets;
    targets.reserve(enemies.size());
    for (std::size_t index = 0; index < enemies.size(); ++index)
    {
        targets.append(enemies[index].get());
    }

    for (std::size_t index = 0; index < playerBullets.size();)
    {
        if (!playerBullets[index].update(
                deltaTime,
                targets,
                collisionAreas,
                worldBounds))
        {
            playerBullets.remove_pos(index);
        }
        else
        {
            ++index;
        }
    }
}

void Level1::drawAmmoCounter(int ammo)
{
    const sf::Vector2f panelPosition(20.f, 46.f);
    sf::RectangleShape panel({108.f, 40.f});
    panel.setPosition(panelPosition);
    panel.setFillColor(sf::Color(25, 25, 25, 220));
    window->draw(panel);

    sf::RectangleShape bulletBody({13.f, 5.f});
    bulletBody.setPosition({32.f, 63.f});
    bulletBody.setFillColor(sf::Color(235, 220, 165));
    window->draw(bulletBody);

    sf::CircleShape bulletTip(2.5f);
    bulletTip.setOrigin({2.5f, 2.5f});
    bulletTip.setPosition({46.f, 65.5f});
    bulletTip.setFillColor(sf::Color(235, 220, 165));
    window->draw(bulletTip);

    constexpr bool segments[10][7] = {
        {true, true, true, true, true, true, false},
        {false, true, true, false, false, false, false},
        {true, true, false, true, true, false, true},
        {true, true, true, true, false, false, true},
        {false, true, true, false, false, true, true},
        {true, false, true, true, false, true, true},
        {true, false, true, true, true, true, true},
        {true, true, true, false, false, false, false},
        {true, true, true, true, true, true, true},
        {true, true, true, true, false, true, true}
    };
    const sf::Vector2f segmentOffsets[7] = {
        {4.f, 0.f}, {14.f, 3.f}, {14.f, 14.f}, {4.f, 24.f},
        {0.f, 14.f}, {0.f, 3.f}, {4.f, 12.f}
    };
    const sf::Vector2f segmentSizes[7] = {
        {11.f, 3.f}, {3.f, 10.f}, {3.f, 10.f}, {11.f, 3.f},
        {3.f, 10.f}, {3.f, 10.f}, {11.f, 3.f}
    };

    const auto drawDigit = [&](int digit, const sf::Vector2f& position)
    {
        for (int segment = 0; segment < 7; ++segment)
        {
            if (!segments[digit][segment])
            {
                continue;
            }

            sf::RectangleShape shape(segmentSizes[segment]);
            shape.setPosition(position + segmentOffsets[segment]);
            shape.setFillColor(sf::Color(245, 220, 110));
            window->draw(shape);
        }
    };

    const int clampedAmmo = std::clamp(ammo, 0, Weapon::MagazineCapacity);
    drawDigit(clampedAmmo / 10, {57.f, 52.f});
    drawDigit(clampedAmmo % 10, {78.f, 52.f});
}

void Level1::spawnEncounter(
    int encounterIndex,
    const LevelEncounter& encounter)
{
    if (activeEncounterIndex >= 0 || !enemies.is_empty())
    {
        throw std::logic_error("No se puede iniciar otra oleada mientras haya enemigos activos");
    }

    activeEncounterIndex = encounterIndex;
    std::size_t spawnIndex = 0;
    while (spawnIndex < encounter.enemies.size())
    {
        const std::string& enemyType = encounter.enemies[spawnIndex].enemyType;
        std::size_t groupEnd = spawnIndex + 1;
        while (groupEnd < encounter.enemies.size() &&
               encounter.enemies[groupEnd].enemyType == enemyType)
        {
            ++groupEnd;
        }

        Designar::DynArray<std::unique_ptr<Enemy>> createdEnemies =
            enemyFactory.createEnemies(enemyType, groupEnd - spawnIndex);
        for (std::size_t index = 0; index < createdEnemies.size(); ++index)
        {
            std::unique_ptr<Enemy>& enemy = createdEnemies[index];
            enemy->setPosition(encounter.enemies[spawnIndex + index].position);
            enemy->setVisualScale(2.5f);

            if (Soldier* soldier = dynamic_cast<Soldier*>(enemy.get()))
            {
                soldier->setCollisionGeometry(collisionAreas, worldBounds);
            }

            enemies.append(std::move(enemy));
        }

        spawnIndex = groupEnd;
    }
}

void Level1::initializeCollisionAreas()
{
    constexpr float mapWidth = 1376.f;
    constexpr float mapHeight = 768.f;
    const sf::Vector2u windowSize = window->getSize();
    const float scaleX = static_cast<float>(windowSize.x) / mapWidth;
    const float scaleY = static_cast<float>(windowSize.y) / mapHeight;
    const sf::FloatRect sourceAreas[] = {
        {{0.f, 0.f}, {360.f, 480.f}},
        {{0.f, 480.f}, {360.f, 90.f}},
        {{350.f, 390.f}, {195.f, 115.f}},
        {{570.f, 385.f}, {155.f, 115.f}},
        {{770.f, 420.f}, {230.f, 115.f}},
        {{1110.f, 0.f}, {266.f, 490.f}},
        {{1100.f, 485.f}, {276.f, 90.f}}
    };

    collisionAreas.clear();
    collisionAreas.reserve(std::size(sourceAreas));
    for (std::size_t index = 0; index < std::size(sourceAreas); ++index)
    {
        const sf::FloatRect& area = sourceAreas[index];
        collisionAreas.append(sf::FloatRect(
            sf::Vector2f(area.position.x * scaleX, area.position.y * scaleY),
            sf::Vector2f(area.size.x * scaleX, area.size.y * scaleY)));
    }
}

bool Level1::collidesAt(const sf::Vector2f& position) const
{
    const sf::Vector2u windowSize = window->getSize();
    constexpr float bodyHalfWidth = 28.f;
    constexpr float bodyHeight = 36.f;

    if (position.x < bodyHalfWidth ||
        position.x > static_cast<float>(windowSize.x) - bodyHalfWidth ||
        position.y < bodyHeight ||
        position.y > static_cast<float>(windowSize.y))
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

void Level1::resolveMovement(Entity& entity, const sf::Vector2f& previousPosition)
{
    const sf::Vector2u windowSize = window->getSize();
    constexpr float bodyHalfWidth = 28.f;
    constexpr float bodyHeight = 36.f;
    const sf::Vector2f target = entity.getPosition();
    sf::Vector2f resolved = previousPosition;

    sf::Vector2f candidate(
        std::clamp(target.x, bodyHalfWidth,
                   static_cast<float>(windowSize.x) - bodyHalfWidth),
        resolved.y);
    if (!collidesAt(candidate))
    {
        resolved.x = candidate.x;
    }

    candidate = {
        resolved.x,
        std::clamp(target.y, bodyHeight, static_cast<float>(windowSize.y))
    };
    if (!collidesAt(candidate))
    {
        resolved.y = candidate.y;
    }

    entity.setPosition(resolved);
}
