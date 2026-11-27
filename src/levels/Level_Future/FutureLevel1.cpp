#include "levels/Level_Future/FutureLevel1.hpp"
#include "Game.hpp"
#include "GameConfig.hpp"
#include "Robot.hpp"
#include "item/Weapon.hpp"
#include "item/Medkit.hpp"
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
           GameConfig::FutureLevel1::MapImageHeight;
}

LevelWorldBounds getWorldBounds(const sf::RenderWindow* window)
{
    const float scale = getWorldScale(window);
    return {
        {0.f, 0.f},
        {
            GameConfig::FutureLevel1::MapImageWidth * scale,
            GameConfig::FutureLevel1::MapImageHeight * scale
        }
    };
}

LevelEncounter createRobotArena(
    const GameConfig::FutureLevel1::ArenaLayout& layout,
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
            std::ceil(std::sqrt(static_cast<float>(layout.robotCount)))));
    const std::size_t rows =
        (layout.robotCount + columns - 1) / columns;
    const float minimumSpawnX = layout.spawnMinimumMapX;
    const float maximumSpawnX = layout.spawnMaximumMapX;
    const float minimumSpawnY = GameConfig::FutureLevel1::EnemyMinimumSpawnMapY;
    const float maximumSpawnY = GameConfig::FutureLevel1::EnemyMaximumSpawnMapY;

    for (std::size_t index = 0; index < layout.robotCount; ++index)
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
            "robot",
            {mapX * scale, mapY * scale}
        });
    }

    return encounter;
}

Designar::DynArray<LevelEncounter> createFutureLevel1Encounters(
    const sf::RenderWindow* window)
{
    const float scale = getWorldScale(window);
    Designar::DynArray<LevelEncounter> encounters;
    encounters.append(createRobotArena(GameConfig::FutureLevel1::FirstArena, scale));
    encounters.append(createRobotArena(GameConfig::FutureLevel1::SecondArena, scale));
    encounters.append(createRobotArena(GameConfig::FutureLevel1::ThirdArena, scale));
    return encounters;
}
}

FutureLevel1::FutureLevel1(sf::RenderWindow* window, Game* game)
    : LevelState(
          window,
          game,
          GameConfig::FutureLevel1::BackgroundPath,
          createFutureLevel1Encounters(window),
          getWorldBounds(window))
{
    const LevelWorldBounds bounds = getWorldBounds(window);
    worldBounds = sf::FloatRect(
        bounds.minimum,
        bounds.maximum - bounds.minimum);

    const float scale = getWorldScale(window);
    player.setPosition({
        GameConfig::FutureLevel1::PlayerStartMapX * scale,
        GameConfig::FutureLevel1::PlayerStartMapY * scale
    });
    player.setVisualScale(GameConfig::FutureLevel1::PlayerVisualScale);
    player.setCombatStats(
        GameConfig::FutureLevel1::PlayerAttackDamage,
        GameConfig::FutureLevel1::PlayerAttackRange,
        GameConfig::FutureLevel1::PlayerAttackCooldown);

    for (const auto& vent : GameConfig::FutureLevel1::RadioactiveVentPositions)
    {
        radioactiveVents.append({vent.mapX * scale, vent.mapY * scale});
    }
    previousPlayerHp = player.getHp();

    auto starterWeapon = std::make_unique<Weapon>();
    starterWeapon->setPosition({
        player.getPosition().x + GameConfig::FutureLevel1::WeaponSpawnOffsetX,
        player.getPosition().y + GameConfig::FutureLevel1::WeaponSpawnOffsetY
    });
    worldWeapons.append(std::move(starterWeapon));
}

Player& FutureLevel1::getPlayer()
{
    return player;
}

sf::Vector2f FutureLevel1::getPlayerPosition() const
{
    return player.getPosition();
}

void FutureLevel1::handleLevelEvent(sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (keyPressed == nullptr || playerDefeated)
    {
        return;
    }

    if (keyPressed->code == sf::Keyboard::Key::F)
    {
        player.useMedkit();
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
                GameConfig::FutureLevel1::WeaponPickupRadius))
        {
            break;
        }
    }
}

void FutureLevel1::updateWorldWeapons(float deltaTime)
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

void FutureLevel1::updateLevel(
    float deltaTime,
    bool,
    std::optional<LevelArenaBounds> activeArenaBounds)
{
    player.update(deltaTime, !playerDefeated);
    clampToWalkableArea(player, activeArenaBounds);

    if (player.isDead() && !playerDefeated)
    {
        playerDefeated = true;
        triggerDefeat();
    }

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
            if (distance <= GameConfig::FutureLevel1::EnemyEntryArrivalDistance)
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

        if (!playerDefeated)
        {
            player.attackTarget(enemy);
        }

        if (enemy.isDead())
        {
            player.addExp(GameConfig::Robot::ExperienceReward);
            spawnEnemyDrop(enemy.getPosition());
            enemies.remove_pos_closing_breach(index);
        }

        else
        {
            ++index;
        }
    }

    updatePlayerBullets(deltaTime);

    const int hpBeforeSteam = player.getHp();
    updateRadioactiveVents(deltaTime);
    const bool damagedBySteam = player.getHp() < hpBeforeSteam;

    if (player.getHp() < previousPlayerHp)
    {
        damageFlashColor = damagedBySteam
            ? sf::Color(90, 230, 70)
            : sf::Color(220, 30, 30);
        damageFlashTimer = 0.22f;
    }

    previousPlayerHp = player.getHp();
    damageFlashTimer = std::max(0.f, damageFlashTimer - deltaTime);

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

void FutureLevel1::drawWorld()
{
    drawRadioactiveVents();

    for (std::size_t i = 0; i < worldWeapons.size(); ++i)
    {
        worldWeapons[i]->draw(*window);
    }

    // Ordenar jugador y enemigos de menor Y (fondo) a mayor Y (frente)
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
                    ? GameConfig::FutureLevel1::EquippedWeaponFacingOffset
                    : -GameConfig::FutureLevel1::EquippedWeaponFacingOffset;
                weapon->setPosition({
                    playerPosition.x + facingOffset,
                    playerPosition.y + GameConfig::FutureLevel1::EquippedWeaponHeightOffset
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

void FutureLevel1::drawHud()
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

void FutureLevel1::updatePlayerWeapon(float deltaTime)
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
                direction * GameConfig::FutureLevel1::PlayerBulletSpawnOffsetX,
            playerPosition.y + GameConfig::FutureLevel1::PlayerBulletSpawnOffsetY),
        direction,
        GameConfig::FutureLevel1::PlayerBulletSpeed,
        static_cast<int>(weapon->getDamage()),
        GameConfig::FutureLevel1::PlayerBulletColor));

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

void FutureLevel1::updatePlayerBullets(float deltaTime)
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
                getNoCollisionAreas(),
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

void FutureLevel1::drawAmmoCounter(int ammo)
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

void FutureLevel1::spawnEncounter(
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
                GameConfig::FutureLevel1::EnemyEntryOffsetMapX * scale;
            if (encounter.arenaBounds.has_value())
            {
                entryX = std::min(
                    entryX,
                    encounter.arenaBounds->maximumX -
                        GameConfig::FutureLevel1::BodyHalfWidth);
            }
            enemy->setPosition({entryX, targetPosition.y});
            enemy->setVisualScale(GameConfig::FutureLevel1::EnemyVisualScale);

            if (Robot* robot = dynamic_cast<Robot*>(enemy.get()))
            {
                robot->setCollisionGeometry(getNoCollisionAreas(), worldBounds);
            }

            enteringEnemies.append({enemy.get(), targetPosition});
            enemies.append(std::move(enemy));
        }

        spawnIndex = groupEnd;
    }
}

void FutureLevel1::clampToWalkableArea(
    Entity& entity,
    const std::optional<LevelArenaBounds>& activeArenaBounds)
{
    constexpr float bodyHalfWidth = GameConfig::FutureLevel1::BodyHalfWidth;
    const float scale = getWorldScale(window);
    const sf::Vector2f target = entity.getPosition();

    float minimumX = worldBounds.position.x + bodyHalfWidth;
    float maximumX =
        worldBounds.position.x + worldBounds.size.x - bodyHalfWidth;
    if (activeArenaBounds.has_value())
    {
        minimumX = std::max(minimumX, activeArenaBounds->minimumX);
        maximumX = std::min(maximumX, activeArenaBounds->maximumX);
    }

    const float minimumY = GameConfig::FutureLevel1::WalkableMinimumMapY * scale;
    const float maximumY = GameConfig::FutureLevel1::WalkableMaximumMapY * scale;

    entity.setPosition({
        std::clamp(target.x, minimumX, maximumX),
        std::clamp(target.y, minimumY, maximumY)
    });
}

void FutureLevel1::updateRadioactiveVents(float deltaTime)
{
    steamAnimTime += deltaTime;
    steamDamageTimer = std::max(0.f, steamDamageTimer - deltaTime);

    const float scale = getWorldScale(window);
    const float radiusX =
        GameConfig::FutureLevel1::SteamHazardRadiusXMap * scale;
    const float radiusY =
        GameConfig::FutureLevel1::SteamHazardRadiusYMap * scale;

    if (!playerDefeated && steamDamageTimer <= 0.f)
    {
        for (std::size_t i = 0; i < radioactiveVents.size(); ++i)
        {
            const sf::Vector2f diff = player.getPosition() - radioactiveVents[i];
            const float normX = diff.x / radiusX;
            const float normY = diff.y / radiusY;
            if (normX * normX + normY * normY <= 1.f)
            {
                player.setHp(std::max(
                    0,
                    player.getHp() - GameConfig::FutureLevel1::SteamDamage));
                steamDamageTimer = GameConfig::FutureLevel1::SteamDamageInterval;
                break;
            }
        }
    }

    for (std::size_t i = 0; i < steamParticles.size();)
    {
        SteamParticle& p = steamParticles[i];
        p.life += deltaTime;
        p.offset.y += p.velocityY * deltaTime;
        p.offset.x += (p.velocityX +
                       std::sin(steamAnimTime * 3.5f + p.phase) * 14.f * scale) *
                      deltaTime;

        if (p.life >= p.maxLife)
        {
            steamParticles.remove_pos(i);
        }
        else
        {
            ++i;
        }
    }

    const std::size_t maxParticles = radioactiveVents.size() * 10;
    if (steamParticles.size() < maxParticles && !radioactiveVents.is_empty())
    {
        const std::size_t ventIndex = steamParticles.size() % radioactiveVents.size();
        const float seed =
            steamAnimTime * 19.7f + static_cast<float>(steamParticles.size()) * 2.3f;
        const float spread = std::sin(seed);
        const float variation = std::abs(std::cos(seed * 1.3f));

        steamParticles.append(SteamParticle{
            radioactiveVents[ventIndex],
            {spread * 28.f * scale, -4.f * scale},
            spread * 10.f * scale,
            -(42.f + variation * 36.f) * scale,
            (10.f + variation * 8.f) * scale,
            0.f,
            1.1f + variation * 0.7f,
            seed
        });
    }
}

void FutureLevel1::drawRadioactiveVents()
{
    const float scale = getWorldScale(window);

    for (std::size_t i = 0; i < radioactiveVents.size(); ++i)
    {
        const sf::Vector2f& pos = radioactiveVents[i];
        const float pulse =
            0.5f + 0.5f * std::sin(steamAnimTime * 4.5f + static_cast<float>(i) * 1.7f);

        const float outerRadius = (56.f + pulse * 10.f) * scale;
        sf::CircleShape outerGlow(outerRadius);
        outerGlow.setOrigin({outerRadius, outerRadius});
        outerGlow.setScale({1.f, 0.36f});
        outerGlow.setPosition(pos);
        outerGlow.setFillColor(sf::Color(
            90,
            235,
            65,
            static_cast<std::uint8_t>(45.f + pulse * 30.f)));
        window->draw(outerGlow);

        const float innerRadius = (32.f + pulse * 6.f) * scale;
        sf::CircleShape innerGlow(innerRadius);
        innerGlow.setOrigin({innerRadius, innerRadius});
        innerGlow.setScale({1.f, 0.35f});
        innerGlow.setPosition(pos);
        innerGlow.setFillColor(sf::Color(
            175,
            255,
            110,
            static_cast<std::uint8_t>(65.f + pulse * 35.f)));
        window->draw(innerGlow);
    }

    for (std::size_t i = 0; i < steamParticles.size(); ++i)
    {
        const SteamParticle& p = steamParticles[i];
        const float progress = std::clamp(p.life / p.maxLife, 0.f, 1.f);
        const float fadeIn = std::min(1.f, progress / 0.18f);
        const float fadeOut = 1.f - progress;
        const float alpha = fadeIn * fadeOut * 145.f;
        const float radius = p.baseRadius * (0.75f + progress * 1.25f);

        sf::CircleShape puff(radius);
        puff.setOrigin({radius, radius});
        puff.setPosition(p.origin + p.offset);
        puff.setFillColor(sf::Color(
            static_cast<std::uint8_t>(145.f - progress * 65.f),
            static_cast<std::uint8_t>(245.f - progress * 55.f),
            static_cast<std::uint8_t>(90.f - progress * 25.f),
            static_cast<std::uint8_t>(std::clamp(alpha, 0.f, 255.f))));
        window->draw(puff);
    }
}

void FutureLevel1::spawnEnemyDrop(const sf::Vector2f& position)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> rollDist(1, 100);
    const int roll = rollDist(rng);

    if (roll <= GameConfig::FutureLevel1::MedkitDropChancePercent)
        {
            auto droppedMedkit = std::make_unique<Medkit>();
            droppedMedkit->setPosition(position);
            worldWeapons.append(std::move(droppedMedkit));
            return;
        }

    if (roll <= GameConfig::FutureLevel1::MedkitDropChancePercent +
                    GameConfig::FutureLevel1::WeaponDropChancePercent)
        {
            auto droppedWeapon = std::make_unique<Weapon>();
            droppedWeapon->setPosition(position);
            worldWeapons.append(std::move(droppedWeapon));
        }
    
}
