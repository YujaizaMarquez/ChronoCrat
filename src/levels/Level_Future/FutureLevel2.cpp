#include "levels/Level_future/FutureLevel2.hpp"
#include "Game.hpp"
#include "GameConfig.hpp"
#include "Infected.hpp"
#include "item/Medkit.hpp"
#include "item/Weapon.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <utility>

namespace
{
const Designar::DynArray<sf::FloatRect>& getNoCollisionAreas()
{
    static const Designar::DynArray<sf::FloatRect> emptyAreas;
    return emptyAreas;
}

float getWorldScale(const sf::RenderWindow* window)
{
    return static_cast<float>(window->getSize().y) /
           GameConfig::FutureLevel2::MapImageHeight;
}

LevelWorldBounds getWorldBounds(const sf::RenderWindow* window)
{
    const float scale = getWorldScale(window);
    return {
        {0.f, 0.f},
        {
            GameConfig::FutureLevel2::MapImageWidth * scale,
            GameConfig::FutureLevel2::MapImageHeight * scale
        }
    };
}

LevelEncounter createInfectedArena(
    const GameConfig::FutureLevel2::ArenaLayout& layout,
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

    const std::size_t totalEnemies = layout.biterCount + layout.macheteCount;
    const std::size_t columns = std::max<std::size_t>(
        1,
        static_cast<std::size_t>(
            std::ceil(std::sqrt(static_cast<float>(totalEnemies)))));
    const std::size_t rows = (totalEnemies + columns - 1) / columns;

    const float minX = layout.spawnMinimumMapX;
    const float maxX = layout.spawnMaximumMapX;
    const float minY = GameConfig::FutureLevel2::EnemyMinimumSpawnMapY;
    const float maxY = GameConfig::FutureLevel2::EnemyMaximumSpawnMapY;

    for (std::size_t index = 0; index < totalEnemies; ++index)
    {
        const std::size_t column = index % columns;
        const std::size_t row = index / columns;
        const float mapX = columns == 1
            ? (minX + maxX) * 0.5f
            : minX + (maxX - minX) *
                  static_cast<float>(column) / static_cast<float>(columns - 1);
        const float mapY = rows == 1
            ? (minY + maxY) * 0.5f
            : minY + (maxY - minY) *
                  static_cast<float>(row) / static_cast<float>(rows - 1);

        const std::string enemyType =
            index < layout.biterCount ? "mordedor" : "machetero";
        encounter.enemies.append({
            enemyType,
            {mapX * scale, mapY * scale}
        });
    }

    return encounter;
}

Designar::DynArray<LevelEncounter> createFutureLevel2Encounters(
    const sf::RenderWindow* window)
{
    const float scale = getWorldScale(window);
    Designar::DynArray<LevelEncounter> encounters;
    encounters.append(createInfectedArena(GameConfig::FutureLevel2::FirstArena, scale));
    encounters.append(createInfectedArena(GameConfig::FutureLevel2::SecondArena, scale));
    encounters.append(createInfectedArena(GameConfig::FutureLevel2::ThirdArena, scale));
    return encounters;
}
}

FutureLevel2::FutureLevel2(sf::RenderWindow* window, Game* game)
    : LevelState(
          window,
          game,
          GameConfig::FutureLevel2::BackgroundPath,
          createFutureLevel2Encounters(window),
          getWorldBounds(window))
{
    const LevelWorldBounds bounds = getWorldBounds(window);
    worldBounds = sf::FloatRect(
        bounds.minimum,
        bounds.maximum - bounds.minimum);

    const float scale = getWorldScale(window);
    player.setPosition({
        GameConfig::FutureLevel2::PlayerStartMapX * scale,
        GameConfig::FutureLevel2::PlayerStartMapY * scale
    });
    player.setVisualScale(GameConfig::FutureLevel2::PlayerVisualScale);
    player.setCombatStats(
        GameConfig::FutureLevel2::PlayerAttackDamage,
        GameConfig::FutureLevel2::PlayerAttackRange,
        GameConfig::FutureLevel2::PlayerAttackCooldown);

    for (const auto& obstacle : GameConfig::FutureLevel2::Barricades)
    {
        if (obstacle.blocksMovement)
        {
            movementObstacles.append(sf::FloatRect(
                {obstacle.bounds.x * scale, obstacle.bounds.y * scale},
                {obstacle.bounds.width * scale, obstacle.bounds.height * scale}));
        }
    }

    for (const auto& barrelPos : GameConfig::FutureLevel2::ExplosiveBarrelPositions)
    {
        explosiveBarrels.append(ExplosiveBarrel{
            {barrelPos.mapX * scale, barrelPos.mapY * scale},
            GameConfig::FutureLevel2::BarrelMaxHp,
            false,
            0.f
        });
    }

    for (std::size_t i = 0; i < GameConfig::FutureLevel2::ElectricHazardPositions.size(); ++i)
    {
        const auto& hazardPos = GameConfig::FutureLevel2::ElectricHazardPositions[i];
        electricHazards.append(ElectricHazard{
            {hazardPos.mapX * scale, hazardPos.mapY * scale},
            static_cast<float>(i) * 1.4f,
            0.f,
            0.f
        });
    }

    previousPlayerHp = player.getHp();

    auto starterWeapon = std::make_unique<Weapon>();
    starterWeapon->setPosition({
        player.getPosition().x + GameConfig::FutureLevel2::WeaponSpawnOffsetX,
        player.getPosition().y + GameConfig::FutureLevel2::WeaponSpawnOffsetY
    });
    worldWeapons.append(std::move(starterWeapon));
}

Player& FutureLevel2::getPlayer()
{
    return player;
}

sf::Vector2f FutureLevel2::getPlayerPosition() const
{
    return player.getPosition();
}

void FutureLevel2::handleLevelEvent(sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (keyPressed == nullptr || playerDefeated)
    {
        return;
    }

    if (keyPressed->code == sf::Keyboard::Key::F)
    {
        const auto countInventoryItems = [this]() -> std::size_t
        {
            Inventory& inv = player.getInventory();
            std::size_t count = 0;
            for (std::size_t i = 0; i < inv.capacity(); ++i)
            {
                if (inv.get(i) != nullptr)
                {
                    ++count;
                }
            }
            return count;
        };

        const std::size_t itemsBefore = countInventoryItems();
        const int hpBefore = player.getHp();
        player.useMedkit();

        // Si se consumió botiquín, también limpia la infección/sangrado
        if (countInventoryItems() < itemsBefore || player.getHp() > hpBefore)
        {
            bleedRemainingTimer = 0.f;
            bleedTickTimer = 0.f;
        }
        return;
    }

    if (keyPressed->code != sf::Keyboard::Key::E ||
        !player.getInventory().hasSpace())
    {
        return;
    }

    for (std::size_t i = 0; i < worldWeapons.size(); ++i)
    {
        if (worldWeapons[i]->tryPickup(
                player.getPosition(),
                GameConfig::FutureLevel2::WeaponPickupRadius))
        {
            break;
        }
    }
}

void FutureLevel2::updateWorldWeapons(float deltaTime)
{
    for (std::size_t i = 0; i < worldWeapons.size();)
    {
        worldWeapons[i]->update(deltaTime, player.getPosition());
        if (worldWeapons[i]->isCollected() && player.getInventory().hasSpace())
        {
            if (player.addItem(std::move(worldWeapons[i])))
            {
                worldWeapons.remove_pos_closing_breach(i);
            }
            else
            {
                ++i;
            }
        }
        else
        {
            ++i;
        }
    }
}

void FutureLevel2::updateLevel(
    float deltaTime,
    bool,
    std::optional<LevelArenaBounds> activeArenaBounds)
{
    player.update(deltaTime, !playerDefeated);
    clampToWalkableArea(player, activeArenaBounds);

    updateWorldWeapons(deltaTime);
    updatePlayerWeapon(deltaTime);

    for (std::size_t index = 0; index < enemies.size();)
    {
        Enemy& enemy = *enemies[index];
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

            const sf::Vector2f difference = entrance.target - enemy.getPosition();
            const float distance =
                std::sqrt(difference.x * difference.x + difference.y * difference.y);
            if (distance <= GameConfig::FutureLevel2::EnemyEntryArrivalDistance)
            {
                enemy.setPosition(entrance.target);
                enemy.move({0.f, 0.f}, deltaTime);
                enteringEnemies.remove_pos_closing_breach(entranceIndex);
            }
            else
            {
                enemy.move(difference / distance, deltaTime);
                clampToWalkableArea(enemy, activeArenaBounds);
                enemy.update(deltaTime);
            }

            entering = true;
            break;
        }

        if (!entering)
        {
            enemy.updateAI(deltaTime, player);
            clampToWalkableArea(enemy, activeArenaBounds);
        }

        if (auto* infected = dynamic_cast<Infected*>(&enemy))
        {
            if (infected->consumeBleedInflicted())
            {
                bleedRemainingTimer = GameConfig::FutureLevel2::BleedDuration;
                if (bleedTickTimer <= 0.f)
                {
                    bleedTickTimer = GameConfig::FutureLevel2::BleedTickInterval;
                }
            }
        }

        if (!playerDefeated)
        {
            player.attackTarget(enemy);
        }

        if (enemy.isDead())
        {
            int reward = GameConfig::Infected::BiterExperienceReward;
            if (const auto* infected = dynamic_cast<const Infected*>(&enemy))
            {
                reward = infected->getExperienceReward();
            }
            player.addExp(reward);
            spawnEnemyDrop(enemy.getPosition());
            enemies.remove_pos_closing_breach(index);
        }
        else
        {
            ++index;
        }
    }

    updatePlayerBullets(deltaTime);
    updateExplosions(deltaTime);

    const int hpBeforeElectric = player.getHp();
    updateElectricHazards(deltaTime);
    const bool damagedByElectric = player.getHp() < hpBeforeElectric;

    const int hpBeforeBleed = player.getHp();
    updateBleedEffect(deltaTime);
    const bool damagedByBleed = player.getHp() < hpBeforeBleed;

    if (player.getHp() < previousPlayerHp)
    {
        if (damagedByElectric)
        {
            damageFlashColor = sf::Color(70, 200, 255);
        }
        else if (damagedByBleed)
        {
            damageFlashColor = sf::Color(185, 35, 130);
        }
        else
        {
            damageFlashColor = sf::Color(220, 30, 30);
        }
        damageFlashTimer = 0.22f;
    }

    previousPlayerHp = player.getHp();
    damageFlashTimer = std::max(0.f, damageFlashTimer - deltaTime);

    if (player.isDead() && !playerDefeated)
    {
        playerDefeated = true;
        triggerDefeat();
    }

    if (activeEncounterIndex >= 0 && enemies.is_empty())
    {
        enteringEnemies.clear();
        completeEncounter(activeEncounterIndex);
        activeEncounterIndex = -1;
        if (areAllEncountersCompleted() && !levelCompleted)
        {
            levelCompleted = true;
            triggerVictory();
        }
    }
}

void FutureLevel2::drawWorld()
{
    drawElectricHazards();
    drawBarricades();
    drawBarrelsAndExplosions();

    for (std::size_t i = 0; i < worldWeapons.size(); ++i)
    {
        worldWeapons[i]->draw(*window);
    }

    Designar::DynArray<Entity*> renderOrder;
    renderOrder.reserve(enemies.size() + 1);
    renderOrder.append(&player);
    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        renderOrder.append(enemies[i].get());
    }

    for (std::size_t i = 1; i < renderOrder.size(); ++i)
    {
        Entity* key = renderOrder[i];
        std::size_t j = i;
        while (j > 0 && renderOrder[j - 1]->getPosition().y > key->getPosition().y)
        {
            renderOrder[j] = renderOrder[j - 1];
            --j;
        }
        renderOrder[j] = key;
    }

    for (std::size_t i = 0; i < renderOrder.size(); ++i)
    {
        renderOrder[i]->draw(*window);
        if (renderOrder[i] == &player)
        {
            if (Weapon* weapon = player.getEquippedWeapon())
            {
                const sf::Vector2f playerPosition = player.getPosition();
                const float facingOffset = player.isFacingRight()
                    ? GameConfig::FutureLevel2::EquippedWeaponFacingOffset
                    : -GameConfig::FutureLevel2::EquippedWeaponFacingOffset;
                weapon->setPosition({
                    playerPosition.x + facingOffset,
                    playerPosition.y + GameConfig::FutureLevel2::EquippedWeaponHeightOffset
                });
                weapon->getAnimation().setFacingRight(player.isFacingRight());
                weapon->drawHeld(*window);
            }
        }
    }

    for (const Bullet& bullet : playerBullets)
    {
        bullet.draw(*window);
    }
}

void FutureLevel2::drawHud()
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

    if (bleedRemainingTimer > 0.f)
    {
        drawBleedIndicator();
    }

    if (damageFlashTimer > 0.f)
    {
        const auto alpha = static_cast<std::uint8_t>((damageFlashTimer / 0.22f) * 110.f);
        sf::RectangleShape flash({
            static_cast<float>(windowSize.x),
            static_cast<float>(windowSize.y)
        });
        flash.setFillColor(sf::Color(
            damageFlashColor.r,
            damageFlashColor.g,
            damageFlashColor.b,
            alpha));
        window->draw(flash);
    }
}

void FutureLevel2::updatePlayerWeapon(float deltaTime)
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
    const sf::Vector2f spawnPos(
        playerPosition.x +
            direction * GameConfig::FutureLevel2::PlayerBulletSpawnOffsetX,
        playerPosition.y + GameConfig::FutureLevel2::PlayerBulletSpawnOffsetY);
    const int bulletDamage = static_cast<int>(weapon->getDamage());

    playerBullets.append(Bullet(
        spawnPos,
        direction,
        GameConfig::FutureLevel2::PlayerBulletSpeed,
        bulletDamage,
        GameConfig::FutureLevel2::PlayerBulletColor));
    playerBulletTrackers.append(BulletTracker{
        spawnPos,
        direction,
        GameConfig::FutureLevel2::PlayerBulletSpeed,
        bulletDamage
    });

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

void FutureLevel2::updatePlayerBullets(float deltaTime)
{
    const float scale = getWorldScale(window);
    const float barrelHalfW = GameConfig::FutureLevel2::BarrelHitHalfWidthMap * scale;
    const float barrelHeight = GameConfig::FutureLevel2::BarrelHitHeightMap * scale;

    Designar::DynArray<Entity*> targets;
    targets.reserve(enemies.size());
    for (std::size_t index = 0; index < enemies.size(); ++index)
    {
        targets.append(enemies[index].get());
    }

    for (std::size_t index = 0; index < playerBullets.size();)
    {
        BulletTracker& tracker = playerBulletTrackers[index];
        const float startX = tracker.position.x;
        const float endX = startX + tracker.direction * tracker.speed * deltaTime;
        const sf::FloatRect sweptBullet(
            {std::min(startX, endX) - GameConfig::Bullet::Radius,
             tracker.position.y - GameConfig::Bullet::Radius},
            {std::abs(endX - startX) + GameConfig::Bullet::Radius * 2.f,
             GameConfig::Bullet::Radius * 2.f});

        bool hitBarrel = false;
        for (std::size_t barrelIdx = 0; barrelIdx < explosiveBarrels.size(); ++barrelIdx)
        {
            ExplosiveBarrel& barrel = explosiveBarrels[barrelIdx];
            if (barrel.exploded)
            {
                continue;
            }

            const sf::FloatRect barrelRect(
                {barrel.position.x - barrelHalfW, barrel.position.y - barrelHeight},
                {barrelHalfW * 2.f, barrelHeight + 12.f * scale});
            if (sweptBullet.findIntersection(barrelRect))
            {
                barrel.hp -= tracker.damage;
                barrel.hitFlashTimer = 0.16f;
                if (barrel.hp <= 0)
                {
                    detonateBarrel(barrelIdx);
                }
                hitBarrel = true;
                break;
            }
        }

        if (hitBarrel)
        {
            playerBullets.remove_pos(index);
            playerBulletTrackers.remove_pos(index);
            continue;
        }

        tracker.position.x = endX;
        if (!playerBullets[index].update(
                deltaTime,
                targets,
                getNoCollisionAreas(),
                worldBounds))
        {
            playerBullets.remove_pos(index);
            playerBulletTrackers.remove_pos(index);
        }
        else
        {
            ++index;
        }
    }
}

void FutureLevel2::detonateBarrel(std::size_t barrelIndex)
{
    if (barrelIndex >= explosiveBarrels.size() || explosiveBarrels[barrelIndex].exploded)
    {
        return;
    }

    ExplosiveBarrel& barrel = explosiveBarrels[barrelIndex];
    barrel.exploded = true;
    barrel.hp = 0;

    const float scale = getWorldScale(window);
    const float radius = GameConfig::FutureLevel2::BarrelExplosionRadiusMap * scale;
    const sf::Vector2f center = barrel.position;

    explosions.append(ExplosionEffect{center, 0.f, 0.36f, radius});

    // Daño en área a los infectados
    for (std::size_t i = 0; i < enemies.size(); ++i)
    {
        const sf::Vector2f diff = enemies[i]->getPosition() - center;
        const float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist <= radius && enemies[i]->getHp() > 0)
        {
            enemies[i]->setHp(std::max(
                0,
                enemies[i]->getHp() - GameConfig::FutureLevel2::BarrelEnemyDamage));
        }
    }

    // Daño al jugador si está demasiado cerca de la explosión
    if (!playerDefeated)
    {
        const sf::Vector2f diff = player.getPosition() - center;
        const float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist <= radius * 0.75f)
        {
            player.setHp(std::max(
                0,
                player.getHp() - GameConfig::FutureLevel2::BarrelPlayerDamage));
        }
    }

    // Reacción en cadena si hay otro barril dentro del radio
    for (std::size_t other = 0; other < explosiveBarrels.size(); ++other)
    {
        if (other == barrelIndex || explosiveBarrels[other].exploded)
        {
            continue;
        }
        const sf::Vector2f diff = explosiveBarrels[other].position - center;
        const float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist <= radius)
        {
            detonateBarrel(other);
        }
    }
}

void FutureLevel2::updateExplosions(float deltaTime)
{
    for (std::size_t i = 0; i < explosiveBarrels.size(); ++i)
    {
        explosiveBarrels[i].hitFlashTimer =
            std::max(0.f, explosiveBarrels[i].hitFlashTimer - deltaTime);
    }

    for (std::size_t i = 0; i < explosions.size();)
    {
        explosions[i].timer += deltaTime;
        if (explosions[i].timer >= explosions[i].duration)
        {
            explosions.remove_pos(i);
        }
        else
        {
            ++i;
        }
    }
}

void FutureLevel2::updateElectricHazards(float deltaTime)
{
    const float scale = getWorldScale(window);
    const float radiusX = GameConfig::FutureLevel2::ElectricRadiusXMap * scale;
    const float radiusY = GameConfig::FutureLevel2::ElectricRadiusYMap * scale;
    const float activeStart =
        GameConfig::FutureLevel2::ElectricOffDuration +
        GameConfig::FutureLevel2::ElectricWarningDuration;
    const float fullCycle =
        activeStart + GameConfig::FutureLevel2::ElectricActiveDuration;

    for (std::size_t i = 0; i < electricHazards.size(); ++i)
    {
        ElectricHazard& hazard = electricHazards[i];
        hazard.cycleTime = std::fmod(hazard.cycleTime + deltaTime, fullCycle);
        hazard.playerDamageTimer = std::max(0.f, hazard.playerDamageTimer - deltaTime);
        hazard.enemyDamageTimer = std::max(0.f, hazard.enemyDamageTimer - deltaTime);

        if (hazard.cycleTime < activeStart)
        {
            continue;
        }

        // Daño al jugador
        if (!playerDefeated && hazard.playerDamageTimer <= 0.f)
        {
            const sf::Vector2f diff = player.getPosition() - hazard.position;
            const float normX = diff.x / radiusX;
            const float normY = diff.y / radiusY;
            if (normX * normX + normY * normY <= 1.f)
            {
                player.setHp(std::max(
                    0,
                    player.getHp() - GameConfig::FutureLevel2::ElectricPlayerDamage));
                hazard.playerDamageTimer = GameConfig::FutureLevel2::ElectricDamageInterval;
            }
        }

        // También electrocuta a los infectados si los atraes hacia la zona
        if (hazard.enemyDamageTimer <= 0.f)
        {
            bool damagedAnyEnemy = false;
            for (std::size_t e = 0; e < enemies.size(); ++e)
            {
                if (enemies[e]->getHp() <= 0)
                {
                    continue;
                }
                const sf::Vector2f diff = enemies[e]->getPosition() - hazard.position;
                const float normX = diff.x / radiusX;
                const float normY = diff.y / radiusY;
                if (normX * normX + normY * normY <= 1.f)
                {
                    enemies[e]->setHp(std::max(
                        0,
                        enemies[e]->getHp() - GameConfig::FutureLevel2::ElectricEnemyDamage));
                    damagedAnyEnemy = true;
                }
            }
            if (damagedAnyEnemy)
            {
                hazard.enemyDamageTimer = GameConfig::FutureLevel2::ElectricDamageInterval;
            }
        }
    }
}

void FutureLevel2::updateBleedEffect(float deltaTime)
{
    if (bleedRemainingTimer <= 0.f || playerDefeated)
    {
        return;
    }

    bleedRemainingTimer = std::max(0.f, bleedRemainingTimer - deltaTime);
    bleedTickTimer -= deltaTime;

    if (bleedTickTimer <= 0.f)
    {
        player.setHp(std::max(
            0,
            player.getHp() - GameConfig::FutureLevel2::BleedDamagePerTick));
        bleedTickTimer = GameConfig::FutureLevel2::BleedTickInterval;
    }
}

void FutureLevel2::drawElectricHazards()
{
    const float scale = getWorldScale(window);
    const float radiusX = GameConfig::FutureLevel2::ElectricRadiusXMap * scale;
    const float radiusY = GameConfig::FutureLevel2::ElectricRadiusYMap * scale;
    const float warningStart = GameConfig::FutureLevel2::ElectricOffDuration;
    const float activeStart =
        warningStart + GameConfig::FutureLevel2::ElectricWarningDuration;

    for (std::size_t i = 0; i < electricHazards.size(); ++i)
    {
        const ElectricHazard& hazard = electricHazards[i];
        const bool isWarning =
            hazard.cycleTime >= warningStart && hazard.cycleTime < activeStart;
        const bool isActive = hazard.cycleTime >= activeStart;

        sf::CircleShape basePlate(radiusX);
        basePlate.setOrigin({radiusX, radiusX});
        basePlate.setScale({1.f, radiusY / radiusX});
        basePlate.setPosition(hazard.position);

        if (isActive)
        {
            const float flicker = 0.5f + 0.5f * std::sin(hazard.cycleTime * 35.f);
            basePlate.setFillColor(sf::Color(
                65,
                215,
                255,
                static_cast<std::uint8_t>(95.f + flicker * 85.f)));
            window->draw(basePlate);

            // Chispas verticales de arco eléctrico
            for (int spark = -1; spark <= 1; ++spark)
            {
                const float offsetX = static_cast<float>(spark) * radiusX * 0.42f;
                const float sparkHeight =
                    (22.f + 16.f * std::abs(std::sin(hazard.cycleTime * 28.f + static_cast<float>(spark)))) *
                    scale;
                sf::RectangleShape bolt({4.f * scale, sparkHeight});
                bolt.setOrigin({2.f * scale, sparkHeight});
                bolt.setPosition({hazard.position.x + offsetX, hazard.position.y});
                bolt.setFillColor(sf::Color(215, 250, 255, 220));
                window->draw(bolt);
            }
        }
        else if (isWarning)
        {
            const float blink = 0.5f + 0.5f * std::sin(hazard.cycleTime * 24.f);
            basePlate.setFillColor(sf::Color(
                250,
                210,
                45,
                static_cast<std::uint8_t>(45.f + blink * 90.f)));
            window->draw(basePlate);
        }
        else
        {
            basePlate.setFillColor(sf::Color(35, 45, 55, 110));
            window->draw(basePlate);
        }

        // Cable central en el suelo
        sf::RectangleShape cable({radiusX * 1.3f, 5.f * scale});
        cable.setOrigin({radiusX * 0.65f, 2.5f * scale});
        cable.setPosition(hazard.position);
        cable.setFillColor(sf::Color(25, 28, 32));
        window->draw(cable);
    }
}

void FutureLevel2::drawBarricades()
{
    const float scale = getWorldScale(window);
    const float visualHeight = 32.f * scale;

    for (std::size_t i = 0; i < movementObstacles.size(); ++i)
    {
        const sf::FloatRect& rect = movementObstacles[i];

        // Sombra en el asfalto
        sf::RectangleShape shadow(rect.size);
        shadow.setPosition(rect.position);
        shadow.setFillColor(sf::Color(15, 18, 22, 140));
        window->draw(shadow);

        // Cuerpo metálico de la barricada
        sf::RectangleShape body({rect.size.x, visualHeight});
        body.setPosition({rect.position.x, rect.position.y + rect.size.y - visualHeight});
        body.setFillColor(sf::Color(78, 86, 94));
        body.setOutlineThickness(2.f);
        body.setOutlineColor(sf::Color(35, 40, 46));
        window->draw(body);

        // Franja de advertencia amarilla superior
        sf::RectangleShape stripe({rect.size.x, 8.f * scale});
        stripe.setPosition({rect.position.x, rect.position.y + rect.size.y - visualHeight + 4.f * scale});
        stripe.setFillColor(sf::Color(230, 185, 40));
        window->draw(stripe);
    }
}

void FutureLevel2::drawBarrelsAndExplosions()
{
    const float scale = getWorldScale(window);
    const float barrelWidth = 34.f * scale;
    const float barrelHeight = 50.f * scale;

    for (std::size_t i = 0; i < explosiveBarrels.size(); ++i)
    {
        const ExplosiveBarrel& barrel = explosiveBarrels[i];
        if (barrel.exploded)
        {
            continue;
        }

        const bool damaged = barrel.hp < GameConfig::FutureLevel2::BarrelMaxHp;
        sf::RectangleShape body({barrelWidth, barrelHeight});
        body.setOrigin({barrelWidth * 0.5f, barrelHeight});
        body.setPosition(barrel.position);
        if (barrel.hitFlashTimer > 0.f)
        {
            body.setFillColor(sf::Color(255, 240, 140));
        }
        else if (damaged)
        {
            body.setFillColor(sf::Color(235, 75, 35));
        }
        else
        {
            body.setFillColor(sf::Color(190, 42, 35));
        }
        body.setOutlineThickness(2.f);
        body.setOutlineColor(sf::Color(55, 18, 16));
        window->draw(body);

        // Franja amarilla de material inflamable
        sf::RectangleShape hazardBand({barrelWidth, 10.f * scale});
        hazardBand.setOrigin({barrelWidth * 0.5f, barrelHeight * 0.58f});
        hazardBand.setPosition(barrel.position);
        hazardBand.setFillColor(sf::Color(245, 200, 50));
        window->draw(hazardBand);
    }

    for (std::size_t i = 0; i < explosions.size(); ++i)
    {
        const ExplosionEffect& exp = explosions[i];
        const float progress = std::clamp(exp.timer / exp.duration, 0.f, 1.f);
        const float currentRadius = exp.maxRadius * (0.25f + 0.75f * progress);
        const auto outerAlpha = static_cast<std::uint8_t>((1.f - progress) * 195.f);

        sf::CircleShape shockwave(currentRadius);
        shockwave.setOrigin({currentRadius, currentRadius});
        shockwave.setPosition(exp.position);
        shockwave.setFillColor(sf::Color(255, 130, 35, outerAlpha));
        window->draw(shockwave);

        const float coreRadius = currentRadius * 0.55f;
        sf::CircleShape core(coreRadius);
        core.setOrigin({coreRadius, coreRadius});
        core.setPosition(exp.position);
        core.setFillColor(sf::Color(255, 245, 150, outerAlpha));
        window->draw(core);
    }
}

void FutureLevel2::drawAmmoCounter(int ammo)
{
    sf::RectangleShape panel({
        GameConfig::Level1::AmmoPanelWidth,
        GameConfig::Level1::AmmoPanelHeight
    });
    panel.setPosition({
        GameConfig::Level1::AmmoPanelX,
        GameConfig::Level1::AmmoPanelY
    });
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
        const auto drawSegment = [&](bool enabled, const GameConfig::Rect& segment)
        {
            if (!enabled)
            {
                return;
            }

            sf::RectangleShape shape({segment.width, segment.height});
            shape.setPosition({position.x + segment.x, position.y + segment.y});
            shape.setFillColor(GameConfig::Level1::AmmoDigitColor);
            window->draw(shape);
        };

        drawSegment(segments[digit][0], GameConfig::Level1::AmmoSegmentTop);
        drawSegment(segments[digit][1], GameConfig::Level1::AmmoSegmentUpperRight);
        drawSegment(segments[digit][2], GameConfig::Level1::AmmoSegmentLowerRight);
        drawSegment(segments[digit][3], GameConfig::Level1::AmmoSegmentBottom);
        drawSegment(segments[digit][4], GameConfig::Level1::AmmoSegmentLowerLeft);
        drawSegment(segments[digit][5], GameConfig::Level1::AmmoSegmentUpperLeft);
        drawSegment(segments[digit][6], GameConfig::Level1::AmmoSegmentMiddle);
    };

    const int clampedAmmo = std::clamp(ammo, 0, GameConfig::Weapon::MagazineCapacity);
    drawDigit(
        clampedAmmo / 10,
        {GameConfig::Level1::AmmoFirstDigitX, GameConfig::Level1::AmmoDigitY});
    drawDigit(
        clampedAmmo % 10,
        {GameConfig::Level1::AmmoSecondDigitX, GameConfig::Level1::AmmoDigitY});
}

void FutureLevel2::drawBleedIndicator()
{
    const float fraction = std::clamp(
        bleedRemainingTimer / GameConfig::FutureLevel2::BleedDuration,
        0.f,
        1.f);
    const sf::Vector2f pos{138.f, 58.f};
    const sf::Vector2f size{102.f, 16.f};

    sf::RectangleShape bg(size);
    bg.setPosition(pos);
    bg.setFillColor(sf::Color(35, 15, 28, 225));
    bg.setOutlineThickness(1.5f);
    bg.setOutlineColor(sf::Color(220, 60, 140));
    window->draw(bg);

    sf::RectangleShape fill({size.x * fraction, size.y});
    fill.setPosition(pos);
    fill.setFillColor(sf::Color(215, 40, 125));
    window->draw(fill);
}

void FutureLevel2::spawnEncounter(
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
                GameConfig::FutureLevel2::EnemyEntryOffsetMapX * scale;
            if (encounter.arenaBounds.has_value())
            {
                entryX = std::min(
                    entryX,
                    encounter.arenaBounds->maximumX -
                        GameConfig::FutureLevel2::BodyHalfWidth);
            }
            enemy->setPosition({entryX, targetPosition.y});
            enemy->setVisualScale(GameConfig::FutureLevel2::EnemyVisualScale);

            enteringEnemies.append({enemy.get(), targetPosition});
            enemies.append(std::move(enemy));
        }

        spawnIndex = groupEnd;
    }
}

void FutureLevel2::clampToWalkableArea(
    Entity& entity,
    const std::optional<LevelArenaBounds>& activeArenaBounds)
{
    constexpr float bodyHalfWidth = GameConfig::FutureLevel2::BodyHalfWidth;
    const float scale = getWorldScale(window);
    const float footHalfHeight = GameConfig::FutureLevel2::FootHalfHeightMap * scale;
    sf::Vector2f pos = entity.getPosition();

    // Resolver colisión con barricadas (empuja por el eje más corto para rodearlas sin trabarse)
    for (std::size_t i = 0; i < movementObstacles.size(); ++i)
    {
        const sf::FloatRect& obstacle = movementObstacles[i];
        const sf::FloatRect entityFoot(
            {pos.x - bodyHalfWidth, pos.y - footHalfHeight},
            {bodyHalfWidth * 2.f, footHalfHeight * 2.f});

        if (const auto overlap = entityFoot.findIntersection(obstacle))
        {
            if (overlap->size.x < overlap->size.y)
            {
                const float obstacleCenterX =
                    obstacle.position.x + obstacle.size.x * 0.5f;
                pos.x += pos.x < obstacleCenterX
                    ? -overlap->size.x
                    : overlap->size.x;
            }
            else
            {
                const float obstacleCenterY =
                    obstacle.position.y + obstacle.size.y * 0.5f;
                pos.y += pos.y < obstacleCenterY
                    ? -overlap->size.y
                    : overlap->size.y;
            }
        }
    }

    float minimumX = worldBounds.position.x + bodyHalfWidth;
    float maximumX =
        worldBounds.position.x + worldBounds.size.x - bodyHalfWidth;
    if (activeArenaBounds.has_value())
    {
        minimumX = std::max(minimumX, activeArenaBounds->minimumX);
        maximumX = std::min(maximumX, activeArenaBounds->maximumX);
    }

    const float minimumY = GameConfig::FutureLevel2::WalkableMinimumMapY * scale;
    const float maximumY = GameConfig::FutureLevel2::WalkableMaximumMapY * scale;

    entity.setPosition({
        std::clamp(pos.x, minimumX, maximumX),
        std::clamp(pos.y, minimumY, maximumY)
    });
}

void FutureLevel2::spawnEnemyDrop(const sf::Vector2f& position)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> rollDist(1, 100);
    const int roll = rollDist(rng);

    if (roll <= GameConfig::FutureLevel2::MedkitDropChancePercent)
    {
        auto droppedMedkit = std::make_unique<Medkit>();
        droppedMedkit->setPosition(position);
        worldWeapons.append(std::move(droppedMedkit));
        return;
    }

    if (roll <= GameConfig::FutureLevel2::MedkitDropChancePercent +
                    GameConfig::FutureLevel2::WeaponDropChancePercent)
    {
        auto droppedWeapon = std::make_unique<Weapon>();
        droppedWeapon->setPosition(position);
        worldWeapons.append(std::move(droppedWeapon));
    }
}