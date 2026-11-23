#include "levels/Level1.hpp"
#include "GameConfig.hpp"
#include "Soldier.hpp"
#include "item/Weapon.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace
{
float getWorldScale(const sf::RenderWindow* window)
{
    return static_cast<float>(window->getSize().y) /
           GameConfig::Level1::MapImageHeight;
}

LevelWorldBounds getWorldBounds(const sf::RenderWindow* window)
{
    const float scale = getWorldScale(window);
    return {
        {0.f, 0.f},
        {
            GameConfig::Level1::MapImageWidth * scale,
            GameConfig::Level1::MapImageHeight * scale
        }
    };
}

LevelEncounter createSoldierArena(
    const GameConfig::Level1::ArenaLayout& layout,
    float scale)
{
    LevelEncounter encounter{
        LevelEncounter::Type::Wave,
        LevelEncounter::Trigger::AtPosition,
        layout.minimumMapX * scale,
        Designar::DynArray<LevelEnemySpawn>()
    };
    encounter.arenaBounds = LevelArenaBounds{
        layout.minimumMapX * scale,
        layout.maximumMapX * scale
    };

    const std::size_t columns = std::max<std::size_t>(
        1,
        static_cast<std::size_t>(
            std::ceil(std::sqrt(static_cast<float>(layout.soldierCount)))));
    const std::size_t rows =
        (layout.soldierCount + columns - 1) / columns;
    const float minimumSpawnX = layout.spawnMinimumMapX;
    const float maximumSpawnX = layout.spawnMaximumMapX;
    const float minimumSpawnY =
        GameConfig::Level1::EnemyMinimumSpawnMapY;
    const float maximumSpawnY =
        GameConfig::Level1::EnemyMaximumSpawnMapY;

    for (std::size_t index = 0;
         index < layout.soldierCount;
         ++index)
    {
        const std::size_t column = index % columns;
        const std::size_t row = index / columns;
        const float mapX = columns == 1
            ? (minimumSpawnX + maximumSpawnX) / 2.f
            : minimumSpawnX +
                  (maximumSpawnX - minimumSpawnX) *
                      static_cast<float>(column) /
                  static_cast<float>(columns - 1);
        const float mapY = rows == 1
            ? (minimumSpawnY + maximumSpawnY) / 2.f
            : minimumSpawnY +
                  (maximumSpawnY - minimumSpawnY) *
                      static_cast<float>(row) /
                  static_cast<float>(rows - 1);

        encounter.enemies.append({
            "soldier",
            {mapX * scale, mapY * scale}
        });
    }

    return encounter;
}

Designar::DynArray<LevelEncounter> createLevel1Encounters(
    const sf::RenderWindow* window)
{
    const float scale = getWorldScale(window);
    Designar::DynArray<LevelEncounter> encounters;
    encounters.append(createSoldierArena(GameConfig::Level1::FirstArena, scale));
    encounters.append(createSoldierArena(GameConfig::Level1::SecondArena, scale));
    encounters.append(createSoldierArena(GameConfig::Level1::ThirdArena, scale));
    return encounters;
}
}

Level1::Level1(sf::RenderWindow* window, Game* game)
    : LevelState(
          window,
          game,
          GameConfig::Level1::BackgroundPath,
          createLevel1Encounters(window),
          getWorldBounds(window))
{
    const LevelWorldBounds bounds = getWorldBounds(window);
    worldBounds = sf::FloatRect(
        bounds.minimum,
        bounds.maximum - bounds.minimum);
    initializeCollisionAreas();
    const float scale = getWorldScale(window);
    player.setPosition({
        GameConfig::Level1::PlayerStartMapX * scale,
        GameConfig::Level1::PlayerStartMapY * scale
    });
    player.setVisualScale(GameConfig::Level1::PlayerVisualScale);
    player.setCombatStats(
        GameConfig::Level1::PlayerAttackDamage,
        GameConfig::Level1::PlayerAttackRange,
        GameConfig::Level1::PlayerAttackCooldown);

    std::unique_ptr<Item> firstWorldWeapon = std::make_unique<Weapon>();
    firstWorldWeapon->setPosition({
        player.getPosition().x + GameConfig::Level1::WeaponSpawnOffsetX,
        player.getPosition().y + GameConfig::Level1::WeaponSpawnOffsetY
    });
    worldWeapons.append(std::move(firstWorldWeapon));

    std::unique_ptr<Item> secondWorldWeapon = std::make_unique<Weapon>();
    secondWorldWeapon->setPosition({
        GameConfig::Level1::SecondWeaponSpawnMapX * scale,
        GameConfig::Level1::SecondWeaponSpawnMapY * scale
    });
    worldWeapons.append(std::move(secondWorldWeapon));
}

const Player& Level1::getPlayer() const
{
    return player;
}

sf::Vector2f Level1::getPlayerPosition() const
{
    return player.getPosition();
}

void Level1::handleLevelEvent(sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (keyPressed == nullptr)
    {
        return;
    }

    if (player.selectInventorySlotFromKey(keyPressed->code))
    {
        return;
    }

    if (keyPressed->code != sf::Keyboard::Key::E ||
        worldWeapons.is_empty() ||
        !player.getInventory().hasSpace() ||
        playerDefeated)
    {
        return;
    }

    for (std::size_t index = 0; index < worldWeapons.size(); ++index)
    {
        worldWeapons[index]->tryPickup(
            player.getPosition(), GameConfig::Level1::WeaponPickupRadius);
    }
}

void Level1::updateLevel(
    float deltaTime,
    bool,
    std::optional<LevelArenaBounds> activeArenaBounds)
{
    const sf::Vector2f previousPlayerPosition = player.getPosition();
    player.update(deltaTime, !playerDefeated);
    resolveMovement(player, previousPlayerPosition, activeArenaBounds);

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
        bool entering = false;
        for (std::size_t entranceIndex = 0;
             entranceIndex < enteringEnemies.size();
             ++entranceIndex)
        {
            EnemyEntrance& entrance = enteringEnemies[entranceIndex];
            if (entrance.enemy != &enemy)
            {
                continue;
            }

            if (enemy.getHp() <= 0)
            {
                enteringEnemies.remove_pos_closing_breach(entranceIndex);
                break;
            }

            const sf::Vector2f difference =
                entrance.target - enemy.getPosition();
            const float distance =
                std::sqrt(difference.x * difference.x +
                          difference.y * difference.y);
            if (distance <= GameConfig::Level1::EnemyEntryArrivalDistance)
            {
                enemy.setPosition(entrance.target);
                enemy.move({0.f, 0.f}, deltaTime);
                enteringEnemies.remove_pos_closing_breach(entranceIndex);
            }
            else
            {
                enemy.move(difference / distance, deltaTime);
                resolveMovement(enemy, previousPosition, activeArenaBounds);
                enemy.update(deltaTime);
            }

            entering = true;
            break;
        }

        if (!entering)
        {
            enemy.updateAI(deltaTime, player);
            resolveMovement(enemy, previousPosition, activeArenaBounds);
        }

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
    for (std::size_t index = 0; index < worldWeapons.size(); ++index)
    {
        worldWeapons[index]->draw(*window);
    }

    player.draw(*window);
    if (Weapon* weapon = player.getEquippedWeapon())
    {
        const sf::Vector2f playerPosition = player.getPosition();
        const float facingOffset = player.isFacingRight()
            ? GameConfig::Level1::EquippedWeaponFacingOffset
            : -GameConfig::Level1::EquippedWeaponFacingOffset;
        weapon->setPosition({
            playerPosition.x + facingOffset,
            playerPosition.y + GameConfig::Level1::EquippedWeaponHeightOffset
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
        const sf::Vector2f size(
            GameConfig::Level1::HealthBarWidth,
            GameConfig::Level1::HealthBarHeight);
        sf::RectangleShape background(size);
        background.setPosition(position);
        background.setFillColor(GameConfig::Level1::HealthBarBackground);
        window->draw(background);

        const float fraction = maxHp > 0
            ? std::clamp(static_cast<float>(hp) / static_cast<float>(maxHp), 0.f, 1.f)
            : 0.f;
        sf::RectangleShape fill({size.x * fraction, size.y});
        fill.setPosition(position);
        fill.setFillColor(color);
        window->draw(fill);
    };

    drawHealthBar(
        {GameConfig::Level1::HealthBarMargin, GameConfig::Level1::HealthBarMargin},
        player.getHp(),
        player.getMaxHp(),
        GameConfig::Level1::PlayerHealthBarColor);
    if (!enemies.is_empty())
    {
        drawHealthBar(
            {static_cast<float>(windowSize.x) -
                 GameConfig::Level1::EnemyHealthBarRightOffset,
             GameConfig::Level1::HealthBarMargin},
            enemies[0]->getHp(),
            enemies[0]->getMaxHp(),
            GameConfig::Level1::EnemyHealthBarColor);
    }

    if (const Weapon* weapon = player.getEquippedWeapon())
    {
        drawAmmoCounter(weapon->getAmmo());
    }
}

void Level1::updateWorldWeapon(float deltaTime)
{
    for (std::size_t index = 0; index < worldWeapons.size();)
    {
        std::unique_ptr<Item>& worldWeapon = worldWeapons[index];
        worldWeapon->update(deltaTime, player.getPosition());
        if (worldWeapon->isCollected())
        {
            if (!player.addItem(std::move(worldWeapon)))
            {
                throw std::logic_error(
                    "No se pudo transferir el arma al inventario");
            }
            worldWeapons.remove_pos_closing_breach(index);
        }
        else
        {
            ++index;
        }
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
            playerPosition.x +
                direction * GameConfig::Level1::PlayerBulletSpawnOffsetX,
            playerPosition.y + GameConfig::Level1::PlayerBulletSpawnOffsetY),
        direction,
        GameConfig::Level1::PlayerBulletSpeed,
        static_cast<int>(weapon->getDamage()),
        GameConfig::Level1::PlayerBulletColor));

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
                projectileCollisionAreas,
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
    const sf::Vector2f panelPosition(
        GameConfig::Level1::AmmoPanelX,
        GameConfig::Level1::AmmoPanelY);
    sf::RectangleShape panel({
        GameConfig::Level1::AmmoPanelWidth,
        GameConfig::Level1::AmmoPanelHeight
    });
    panel.setPosition(panelPosition);
    panel.setFillColor(GameConfig::Level1::AmmoPanelColor);
    window->draw(panel);

    sf::RectangleShape bulletBody({
        GameConfig::Level1::AmmoIconBodyWidth,
        GameConfig::Level1::AmmoIconBodyHeight
    });
    bulletBody.setPosition({
        GameConfig::Level1::AmmoIconBodyX,
        GameConfig::Level1::AmmoIconBodyY
    });
    bulletBody.setFillColor(GameConfig::Level1::AmmoIconColor);
    window->draw(bulletBody);

    sf::CircleShape bulletTip(GameConfig::Level1::AmmoIconTipRadius);
    bulletTip.setOrigin({
        GameConfig::Level1::AmmoIconTipRadius,
        GameConfig::Level1::AmmoIconTipRadius
    });
    bulletTip.setPosition({
        GameConfig::Level1::AmmoIconTipX,
        GameConfig::Level1::AmmoIconTipY
    });
    bulletTip.setFillColor(GameConfig::Level1::AmmoIconColor);
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
    const auto drawDigit = [&](int digit, const sf::Vector2f& position)
    {
        const auto drawSegment = [&](bool enabled,
                                     const GameConfig::Rect& segment)
        {
            if (!enabled)
            {
                return;
            }

            sf::RectangleShape shape({segment.width, segment.height});
            shape.setPosition({
                position.x + segment.x,
                position.y + segment.y
            });
            shape.setFillColor(GameConfig::Level1::AmmoDigitColor);
            window->draw(shape);
        };

        drawSegment(segments[digit][0], GameConfig::Level1::AmmoSegmentTop);
        drawSegment(
            segments[digit][1],
            GameConfig::Level1::AmmoSegmentUpperRight);
        drawSegment(
            segments[digit][2],
            GameConfig::Level1::AmmoSegmentLowerRight);
        drawSegment(segments[digit][3], GameConfig::Level1::AmmoSegmentBottom);
        drawSegment(
            segments[digit][4],
            GameConfig::Level1::AmmoSegmentLowerLeft);
        drawSegment(
            segments[digit][5],
            GameConfig::Level1::AmmoSegmentUpperLeft);
        drawSegment(segments[digit][6], GameConfig::Level1::AmmoSegmentMiddle);
    };

    const int clampedAmmo =
        std::clamp(ammo, 0, GameConfig::Weapon::MagazineCapacity);
    drawDigit(
        clampedAmmo / 10,
        {GameConfig::Level1::AmmoFirstDigitX, GameConfig::Level1::AmmoDigitY});
    drawDigit(
        clampedAmmo % 10,
        {GameConfig::Level1::AmmoSecondDigitX, GameConfig::Level1::AmmoDigitY});
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
            const sf::Vector2f targetPosition =
                encounter.enemies[spawnIndex + index].position;
            const float scale = getWorldScale(window);
            float entryX = targetPosition.x +
                GameConfig::Level1::EnemyEntryOffsetMapX * scale;
            if (encounter.arenaBounds.has_value())
            {
                entryX = std::min(
                    entryX,
                    encounter.arenaBounds->maximumX -
                        GameConfig::Level1::BodyHalfWidth);
            }
            enemy->setPosition({entryX, targetPosition.y});
            enemy->setVisualScale(GameConfig::Level1::EnemyVisualScale);

            if (Soldier* soldier = dynamic_cast<Soldier*>(enemy.get()))
            {
                soldier->setCollisionGeometry(
                    projectileCollisionAreas, worldBounds);
                soldier->setSquadSlot(index, groupEnd - spawnIndex);
            }

            enteringEnemies.append({enemy.get(), targetPosition});
            enemies.append(std::move(enemy));
        }

        spawnIndex = groupEnd;
    }
}

void Level1::initializeCollisionAreas()
{
    const float scale = getWorldScale(window);
    movementCollisionAreas.clear();
    projectileCollisionAreas.clear();
    movementCollisionAreas.reserve(
        GameConfig::Level1::CollisionObstacles.size());
    projectileCollisionAreas.reserve(
        GameConfig::Level1::CollisionObstacles.size());

    const auto appendObstacle = [&](const GameConfig::CollisionObstacle& obstacle)
    {
        const GameConfig::Rect& bounds = obstacle.bounds;
        const sf::FloatRect area(
            {bounds.x * scale, bounds.y * scale},
            {bounds.width * scale, bounds.height * scale});
        if (obstacle.blocksMovement)
        {
            movementCollisionAreas.append(area);
        }
        if (obstacle.blocksProjectiles)
        {
            projectileCollisionAreas.append(area);
        }
    };
    for (const GameConfig::CollisionObstacle& obstacle :
         GameConfig::Level1::CollisionObstacles)
    {
        appendObstacle(obstacle);
    }
}

bool Level1::collidesAt(const sf::Vector2f& position) const
{
    constexpr float bodyHalfWidth = GameConfig::Level1::BodyHalfWidth;
    constexpr float bodyHeight = GameConfig::Level1::BodyHeight;
    const float scale = getWorldScale(window);
    const float minimumY = GameConfig::Level1::WalkableMinimumMapY * scale;
    const float maximumY = GameConfig::Level1::WalkableMaximumMapY * scale;
    const float minimumX = worldBounds.position.x + bodyHalfWidth;
    const float maximumX =
        worldBounds.position.x + worldBounds.size.x - bodyHalfWidth;

    if (position.x < minimumX ||
        position.x > maximumX ||
        position.y < minimumY ||
        position.y > maximumY)
    {
        return true;
    }

    const sf::FloatRect body(
        {position.x - bodyHalfWidth, position.y - bodyHeight},
        {bodyHalfWidth * 2.f, bodyHeight});
    for (const sf::FloatRect& area : movementCollisionAreas)
    {
        if (area.findIntersection(body))
        {
            return true;
        }
    }

    return false;
}

void Level1::resolveMovement(
    Entity& entity,
    const sf::Vector2f& previousPosition,
    const std::optional<LevelArenaBounds>& activeArenaBounds)
{
    constexpr float bodyHalfWidth = GameConfig::Level1::BodyHalfWidth;
    const float scale = getWorldScale(window);
    const sf::Vector2f target = entity.getPosition();
    sf::Vector2f resolved = previousPosition;
    float minimumX = worldBounds.position.x + bodyHalfWidth;
    float maximumX =
        worldBounds.position.x + worldBounds.size.x - bodyHalfWidth;
    if (activeArenaBounds.has_value())
    {
        minimumX = std::max(minimumX, activeArenaBounds->minimumX);
        maximumX = std::min(maximumX, activeArenaBounds->maximumX);
    }
    const float minimumY = GameConfig::Level1::WalkableMinimumMapY * scale;
    const float maximumY = GameConfig::Level1::WalkableMaximumMapY * scale;

    sf::Vector2f candidate(
        std::clamp(target.x, minimumX, maximumX),
        resolved.y);
    if (!collidesAt(candidate))
    {
        resolved.x = candidate.x;
    }

    candidate = {
        resolved.x,
        std::clamp(target.y, minimumY, maximumY)
    };
    if (!collidesAt(candidate))
    {
        resolved.y = candidate.y;
    }

    entity.setPosition(resolved);
}
