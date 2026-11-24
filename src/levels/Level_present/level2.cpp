
#include "levels/Level_present/level2.hpp"
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
           GameConfig::Level2::MapImageHeight;
}

// Convierte los limites Y del mapa a coordenadas del mundo.
// Los valores 740 y 1080 son coordenadas originales del mapa.
float getMinWalkableY(const sf::RenderWindow* window)
{
    return Level2::MinWalkableY * getWorldScale(window);
}

float getMaxWalkableY(const sf::RenderWindow* window)
{
    return Level2::MaxWalkableY * getWorldScale(window);
}

LevelWorldBounds getWorldBounds(const sf::RenderWindow* window)
{
    const float scale = getWorldScale(window);

    return {
        {0.f, 0.f},
        {
            GameConfig::Level2::MapImageWidth * scale,
            GameConfig::Level2::MapImageHeight * scale
        }
    };
}

Designar::DynArray<LevelEncounter> createLevel2Encounters(
    const sf::RenderWindow* window)
{
    const float scale = getWorldScale(window);
    const float minY = getMinWalkableY(window);
    const float maxY = getMaxWalkableY(window);

    Designar::DynArray<LevelEncounter> encounters;

    LevelEncounter wave1{
        LevelEncounter::Type::Wave,
        LevelEncounter::Trigger::AtPosition,
        GameConfig::Level2::FirstWaveTriggerX * scale,
        Designar::DynArray<LevelEnemySpawn>()
    };

    wave1.arenaBounds = LevelArenaBounds{
        GameConfig::Level2::FirstWaveMinX * scale,
        GameConfig::Level2::FirstWaveMaxX * scale
    };

    wave1.enemies.append(LevelEnemySpawn{
        "cientifico1",
        sf::Vector2f{
            GameConfig::Level2::FirstWaveSpawnX * scale,
            std::clamp(
                GameConfig::Level2::FirstWaveSpawnY * scale,
                minY,
                maxY
            )
        }
    });

    encounters.append(std::move(wave1));

    return encounters;
}

} // namespace


Level2::Level2(sf::RenderWindow* window, Game* game)
    : LevelState(
          window,
          game,
          GameConfig::Level2::BackgroundPath,
          createLevel2Encounters(window),
          getWorldBounds(window))
{
    const LevelWorldBounds bounds = getWorldBounds(window);

    worldBounds = sf::FloatRect(
        bounds.minimum,
        bounds.maximum - bounds.minimum
    );

    initializeCollisionAreas();
    initializeAnimatedObjects();

    const float scale = getWorldScale(window);
    initializeSerpent(scale);
    const float minY = getMinWalkableY(window);
    const float maxY = getMaxWalkableY(window);

    // Posicion inicial del jugador.
    const sf::Vector2f startPos{
        GameConfig::Level2::PlayerStartMapX * scale,
        std::clamp(
            GameConfig::Level2::PlayerStartMapY * scale,
            minY,
            maxY
        )
    };

    player.setPosition(startPos);

    player.setVisualScale(
        GameConfig::Level2::PlayerVisualScale
    );

    player.configureSpriteAnimations(
        GameConfig::Level2::PlayerWalkSpritePath,
        GameConfig::Level2::PlayerMaskedWalkSpritePath,
        GameConfig::Level2::PlayerAttackSpritePath,
        GameConfig::Level2::PlayerDeathSpritePath
    );

    player.setCombatStats(
        GameConfig::Level2::PlayerAttackDamage,
        GameConfig::Level2::PlayerAttackRange,
        GameConfig::Level2::PlayerAttackCooldown
    );

    // Cargar el cientifico que el jugador puede rescatar.
    cientifico1Loaded = cientifico1.loadFromJson(
        "data/cientifico1.json"
    );

    if (!cientifico1Loaded)
    {
        throw std::runtime_error(
            "No se pudo cargar cientifico1 desde JSON"
        );
    }

    cientifico1.setVisualScale(1.f);

    cientifico1.setPosition(sf::Vector2f{
        GameConfig::Level2::Cientifico1StartMapX * scale,
        std::clamp(
            GameConfig::Level2::Cientifico1StartMapY * scale,
            minY,
            maxY
        )
    });

    cientifico1.playAnimation("idle");
}


void Level2::initializeAnimatedObjects()
{
    const float scale = getWorldScale(window);

    tapabocas = std::make_unique<Mask>();
    if (!tapabocas->loadFromFile(
            "assets/level_pandemia/tapabocas_girando.png",
            sf::Vector2f{
                GameConfig::Level2::TapabocasMapX * scale,
                GameConfig::Level2::TapabocasMapY * scale
            },
            sf::Vector2f{3.f, 3.f}))
    {
        throw std::runtime_error(
            "No se pudo cargar tapabocas_girando.png");
    }

    if (!tuboVirusTexture.loadFromFile(
            "assets/level_pandemia/tubo_virus_transparente.png"))
    {
        throw std::runtime_error(
            "No se pudo cargar tubo_virus_transparente.png");
    }

    tuboVirusTexture.setSmooth(false);

    tuboVirusSprite.emplace(tuboVirusTexture);

    // Tubo: 5 frames de 64x320
    tuboVirusSprite->setTextureRect(
        sf::IntRect({0, 0}, {64, 320}));

    tuboVirusSprite->setPosition(
        sf::Vector2f(1190.f * scale, 800.f * scale));

    tuboVirusSprite->setScale({3.f, 3.f});
}


void Level2::initializeSerpent(float scale)
{
    auto created = enemyFactory.createEnemies("serpiente_murcielago", 1);
    serpiente = std::move(created[0]);

    serpiente->setCombatStats(
        GameConfig::Level2::SerpentDetectionRange * scale,
        GameConfig::Level2::SerpentAttackRange * scale,
        GameConfig::Level2::SerpentAttackDamage,
        GameConfig::Level2::SerpentAttackCooldown,
        GameConfig::Enemy::ExperienceReward);
    serpiente->setPosition(sf::Vector2f{
        GameConfig::Level2::SerpentMapX * scale,
        GameConfig::Level2::SerpentMapY * scale
    });
}


void Level2::updateAnimatedObjects(float deltaTime)
{
    tuboVirusAnimationTimer += deltaTime;

    if (tapabocas)
        tapabocas->update(deltaTime);

    if (tuboVirusAnimationTimer >= ObjectAnimationSpeed)
    {
        tuboVirusAnimationTimer = 0.f;
        tuboVirusFrame = (tuboVirusFrame + 1) % TotalObjectFrames;

        tuboVirusSprite->setTextureRect(
    sf::IntRect({tuboVirusFrame * 64, 0}, {64, 320})
);
    }
}

void Level2::drawAnimatedObjects()
{
    if (tapabocas)
        tapabocas->draw(*window);

    if (tuboVirusSprite.has_value())
        window->draw(*tuboVirusSprite);
}


void Level2::updateSerpent(float deltaTime)
{
    if (!serpiente || serpiente->isDead())
    {
        return;
    }

    const sf::Vector2f previousPosition = serpiente->getPosition();
    serpiente->updateAI(deltaTime, player);
    resolveMovement(*serpiente, previousPosition, std::nullopt);
}


Player& Level2::getPlayer()
{
    return player;
}

sf::Vector2f Level2::getPlayerPosition() const
{
    return player.getPosition();
}


void Level2::handleLevelEvent(sf::Event& event)
{
    const auto* keyPressed =
        event.getIf<sf::Event::KeyPressed>();

    if (keyPressed == nullptr || playerDefeated)
    {
        return;
    }

    if (keyPressed->code == sf::Keyboard::Key::E &&
        cientifico1Loaded)
    {
        const sf::Vector2f diff =
            cientifico1.getPosition() - player.getPosition();

        const float dist = std::sqrt(
            diff.x * diff.x + diff.y * diff.y
        );

        if (dist <= GameConfig::Level2::Cientifico1InteractRadius)
        {
            cientifico1RescuedOrReached = true;
        }
    }
}


void Level2::updateLevel(
    float deltaTime,
    bool,
    std::optional<LevelArenaBounds> activeArenaBounds)
{
    updateAnimatedObjects(deltaTime);
    const float minY = getMinWalkableY(window);
    const float maxY = getMaxWalkableY(window);

    // Actualizar jugador y resolver sus colisiones.
    const sf::Vector2f previousPlayerPosition =
        player.getPosition();

    player.update(deltaTime, !playerDefeated);

    if (maskTimeRemaining > 0.f)
    {
        maskTimeRemaining = std::max(0.f, maskTimeRemaining - deltaTime);
        if (maskTimeRemaining <= 0.f)
        {
            player.setWearingMask(false);
        }
    }

    if (tapabocas && !playerDefeated && !player.isDead())
    {
        const sf::Vector2f difference =
            tapabocas->getPosition() - player.getPosition();
        const float distance = std::sqrt(
            difference.x * difference.x +
            difference.y * difference.y
        );

        if (distance <= GameConfig::Level2::TapabocasEquipRadius)
        {
            player.setWearingMask(true);
            maskTimeRemaining = GameConfig::Level2::MaskDuration;
            tapabocas.reset();
        }
    }

    resolveMovement(
        player,
        previousPlayerPosition,
        std::nullopt
    );

    if (player.isDead())
    {
        playerDefeated = true;
    }

    updateSerpent(deltaTime);
    updateCientifico1(deltaTime);
    updatePlayerWeapon(deltaTime);

    // Actualizar enemigos.
    for (std::size_t index = 0; index < enemies.size();)
    {
        Enemy& enemy = *enemies[index];

        const sf::Vector2f previousPosition =
            enemy.getPosition();

        bool entering = false;

        // Comprobar si el enemigo todavia esta entrando al escenario.
        for (std::size_t entranceIndex = 0;
             entranceIndex < enteringEnemies.size();
             ++entranceIndex)
        {
            EnteringEnemy& entrance =
                enteringEnemies[entranceIndex];

            if (entrance.enemy != &enemy)
            {
                continue;
            }

            if (enemy.getHp() <= 0)
            {
                enteringEnemies.remove_pos_closing_breach(
                    entranceIndex
                );

                break;
            }

            const sf::Vector2f clampedTarget{
                entrance.target.x,
                std::clamp(
                    entrance.target.y,
                    minY,
                    maxY
                )
            };

            const sf::Vector2f difference =
                clampedTarget - enemy.getPosition();

            const float distance = std::sqrt(
                difference.x * difference.x +
                difference.y * difference.y
            );

            if (distance <=
                GameConfig::Level2::EnemyEntryArrivalDistance)
            {
                enemy.setPosition(clampedTarget);

                resolveMovement(
                    enemy,
                    previousPosition,
                    activeArenaBounds
                );

                enemy.move({0.f, 0.f}, deltaTime);

                enteringEnemies.remove_pos_closing_breach(
                    entranceIndex
                );
            }
            else
            {
                enemy.move(
                    difference / distance,
                    deltaTime
                );

                resolveMovement(
                    enemy,
                    previousPosition,
                    activeArenaBounds
                );

                enemy.update(deltaTime);
            }

            entering = true;
            break;
        }

        // Cuando termina de entrar, comienza su inteligencia artificial.
        if (!entering)
        {
            enemy.updateAI(deltaTime, player);

            resolveMovement(
                enemy,
                previousPosition,
                activeArenaBounds
            );
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

    // Finalizar encuentro cuando no queden enemigos.
    if (activeEncounterIndex >= 0 && enemies.is_empty())
    {
        completeEncounter(activeEncounterIndex);
        activeEncounterIndex = -1;
    }

    // Completar nivel cuando se hayan terminado los encuentros
    // y el jugador haya llegado al cientifico.
    if (areAllEncountersCompleted() &&
        cientifico1RescuedOrReached)
    {
        completeLevelProgress();
    }
}


void Level2::updateCientifico1(float deltaTime)
{
    if (!cientifico1Loaded)
    {
        return;
    }

    const sf::Vector2f previousPos =
        cientifico1.getPosition();

    if (cientifico1RescuedOrReached)
    {
        const sf::Vector2f toPlayer =
            player.getPosition() - previousPos;

        const float dist = std::sqrt(
            toPlayer.x * toPlayer.x +
            toPlayer.y * toPlayer.y
        );

        if (dist > GameConfig::Level2::Cientifico1FollowDistance)
        {
            cientifico1.move(
                toPlayer / dist,
                deltaTime
            );

            cientifico1.playAnimation("walk");
        }
        else
        {
            cientifico1.playAnimation("idle");
        }
    }

    cientifico1.update(deltaTime);

    resolveCientifico1Movement(previousPos);
}


void Level2::drawWorld()
{
    drawAnimatedObjects();
    if (cientifico1Loaded)
    {
        cientifico1.draw(*window);
    }

    if (serpiente && !serpiente->isDead())
    {
        serpiente->draw(*window);
    }
    player.draw(*window);

    if (Weapon* weapon = player.getEquippedWeapon())
    {
        const sf::Vector2f playerPosition =
            player.getPosition();

        const float facingOffset = player.isFacingRight()
            ? GameConfig::Level2::EquippedWeaponFacingOffset
            : -GameConfig::Level2::EquippedWeaponFacingOffset;

        weapon->setPosition(sf::Vector2f{
            playerPosition.x + facingOffset,
            playerPosition.y +
                GameConfig::Level2::EquippedWeaponHeightOffset
        });

        weapon->getAnimation().setFacingRight(
            player.isFacingRight()
        );

        weapon->drawHeld(*window);
    }

    for (std::size_t index = 0;
         index < enemies.size();
         ++index)
    {
        enemies[index]->draw(*window);
    }

    for (const Bullet& bullet : playerBullets)
    {
        bullet.draw(*window);
    }
}


void Level2::drawHud()
{
    const sf::Vector2u windowSize =
        window->getSize();

    const auto drawHealthBar =
        [this](
            const sf::Vector2f& position,
            int hp,
            int maxHp,
            const sf::Color& color)
    {
        const sf::Vector2f size(
            GameConfig::Level2::HealthBarWidth,
            GameConfig::Level2::HealthBarHeight
        );

        sf::RectangleShape background(size);

        background.setPosition(position);

        background.setFillColor(
            GameConfig::Level2::HealthBarBackground
        );

        window->draw(background);

        const float fraction = maxHp > 0
            ? std::clamp(
                static_cast<float>(hp) /
                static_cast<float>(maxHp),
                0.f,
                1.f
            )
            : 0.f;

        sf::RectangleShape fill(
            sf::Vector2f{
                size.x * fraction,
                size.y
            }
        );

        fill.setPosition(position);
        fill.setFillColor(color);

        window->draw(fill);
    };

    // Barra de vida del jugador.
    drawHealthBar(
        sf::Vector2f{
            GameConfig::Level2::HealthBarMargin,
            GameConfig::Level2::HealthBarMargin
        },
        player.getHp(),
        player.getMaxHp(),
        GameConfig::Level2::PlayerHealthBarColor
    );

    // Barra de vida del primer enemigo.
    if (!enemies.is_empty())
    {
        drawHealthBar(
            sf::Vector2f{
                static_cast<float>(windowSize.x) -
                    GameConfig::Level2::EnemyHealthBarRightOffset,
                GameConfig::Level2::HealthBarMargin
            },
            enemies[0]->getHp(),
            enemies[0]->getMaxHp(),
            GameConfig::Level2::EnemyHealthBarColor
        );
    }
}


void Level2::updatePlayerWeapon(float deltaTime)
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

    const sf::Vector2f playerPosition =
        player.getPosition();

    const float direction =
        player.isFacingRight() ? 1.f : -1.f;

    playerBullets.append(Bullet(
        sf::Vector2f(
            playerPosition.x +
                direction *
                GameConfig::Level2::PlayerBulletSpawnOffsetX,

            playerPosition.y +
                GameConfig::Level2::PlayerBulletSpawnOffsetY
        ),
        direction,
        GameConfig::Level2::PlayerBulletSpeed,
        static_cast<int>(weapon->getDamage()),
        GameConfig::Level2::PlayerBulletColor
    ));
}


void Level2::updatePlayerBullets(float deltaTime)
{
    Designar::DynArray<Entity*> targets;

    targets.reserve(enemies.size());
    if (serpiente && !serpiente->isDead())
    {
        targets.reserve(enemies.size() + 1);
    }

    for (std::size_t index = 0;
         index < enemies.size();
         ++index)
    {
        targets.append(enemies[index].get());
    }

    if (serpiente && !serpiente->isDead())
    {
        targets.append(serpiente.get());
    }

    for (std::size_t index = 0;
         index < playerBullets.size();)
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


void Level2::spawnEncounter(
    int encounterIndex,
    const LevelEncounter& encounter)
{
    if (activeEncounterIndex >= 0 ||
        !enemies.is_empty())
    {
        throw std::logic_error(
            "No se puede iniciar otra oleada mientras haya enemigos activos"
        );
    }

    activeEncounterIndex = encounterIndex;

    const float scale = getWorldScale(window);
    const float minY = getMinWalkableY(window);
    const float maxY = getMaxWalkableY(window);

    for (std::size_t i = 0;
         i < encounter.enemies.size();
         ++i)
    {
        auto created = enemyFactory.createEnemies(
            encounter.enemies[i].enemyType,
            1
        );

        if (created.is_empty())
        {
            continue;
        }

        std::unique_ptr<Enemy> enemy =
            std::move(created[0]);

        const sf::Vector2f targetPosition{
            encounter.enemies[i].position.x,
            std::clamp(
                encounter.enemies[i].position.y,
                minY,
                maxY
            )
        };

        float entryX =
            targetPosition.x +
            GameConfig::Level2::EnemyEntryOffsetMapX * scale;

        if (encounter.arenaBounds.has_value())
        {
            entryX = std::min(
                entryX,
                encounter.arenaBounds->maximumX -
                    GameConfig::Level2::BodyHalfWidth
            );
        }

        enemy->setPosition(sf::Vector2f{
            entryX,
            std::clamp(
                targetPosition.y,
                minY,
                maxY
            )
        });

        enemy->setVisualScale(
    GameConfig::Level2::PlayerVisualScale
        );

        // Configuracion de colisiones para Soldier.
        if (Soldier* soldier =
                dynamic_cast<Soldier*>(enemy.get()))
        {
            soldier->setCollisionGeometry(
                projectileCollisionAreas,
                worldBounds
            );
        }

        enteringEnemies.append({
            enemy.get(),
            targetPosition
        });

        enemies.append(std::move(enemy));
    }
}


void Level2::initializeCollisionAreas()
{
    const float scale = getWorldScale(window);

    movementCollisionAreas.clear();
    projectileCollisionAreas.clear();

    const auto appendObstacle =
        [&](const GameConfig::CollisionObstacle& obstacle)
    {
        const GameConfig::Rect& bounds =
            obstacle.bounds;

        const sf::FloatRect area(
            {
                bounds.x * scale,
                bounds.y * scale
            },
            {
                bounds.width * scale,
                bounds.height * scale
            }
        );

        if (obstacle.blocksMovement)
        {
            movementCollisionAreas.append(area);
        }

        if (obstacle.blocksProjectiles)
        {
            projectileCollisionAreas.append(area);
        }
    };

    for (const auto& obstacle :
         GameConfig::Level2::Obstacles)
    {
        appendObstacle(obstacle);
    }
}


bool Level2::collidesAt(
    const sf::Vector2f& position) const
{
    constexpr float bodyHalfWidth =
        GameConfig::Level2::BodyHalfWidth;

    constexpr float bodyHeight =
        GameConfig::Level2::BodyHeight;

    const float minY = getMinWalkableY(window);
    const float maxY = getMaxWalkableY(window);

    const float minimumX =
        worldBounds.position.x + bodyHalfWidth;

    const float maximumX =
        worldBounds.position.x +
        worldBounds.size.x -
        bodyHalfWidth;

    // Limites del escenario.
    if (position.x < minimumX ||
        position.x > maximumX ||
        position.y < minY ||
        position.y > maxY)
    {
        return true;
    }

    // Rectangulo de colision del personaje.
    const sf::FloatRect body(
        {
            position.x - bodyHalfWidth,
            position.y - bodyHeight
        },
        {
            bodyHalfWidth * 2.f,
            bodyHeight
        }
    );

    // Comprobar colisiones con los obstaculos.
    for (const sf::FloatRect& area :
         movementCollisionAreas)
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
    const sf::Vector2f& previousPosition,
    const std::optional<LevelArenaBounds>& activeArenaBounds)
{
    constexpr float bodyHalfWidth =
        GameConfig::Level2::BodyHalfWidth;

    const float minY = getMinWalkableY(window);
    const float maxY = getMaxWalkableY(window);

    const sf::Vector2f target =
        entity.getPosition();

    sf::Vector2f resolved =
        previousPosition;

    float minimumX =
        worldBounds.position.x + bodyHalfWidth;

    float maximumX =
        worldBounds.position.x +
        worldBounds.size.x -
        bodyHalfWidth;

    // Si existe una arena activa, limitar el movimiento en X.
    if (activeArenaBounds.has_value())
    {
        minimumX = std::max(
            minimumX,
            activeArenaBounds->minimumX
        );

        maximumX = std::min(
            maximumX,
            activeArenaBounds->maximumX
        );
    }

    // Resolver primero el movimiento horizontal.
    sf::Vector2f candidateX(
        std::clamp(
            target.x,
            minimumX,
            maximumX
        ),
        std::clamp(
            resolved.y,
            minY,
            maxY
        )
    );

    if (!collidesAt(candidateX))
    {
        resolved.x = candidateX.x;
    }

    // Resolver despues el movimiento vertical.
    sf::Vector2f candidateY(
        resolved.x,
        std::clamp(
            target.y,
            minY,
            maxY
        )
    );

    if (!collidesAt(candidateY))
    {
        resolved.y = candidateY.y;
    }
    else
    {
        resolved.y = std::clamp(
            resolved.y,
            minY,
            maxY
        );
    }

    entity.setPosition(resolved);
}


void Level2::resolveCientifico1Movement(
    const sf::Vector2f& previousPosition)
{
    constexpr float bodyHalfWidth =
        GameConfig::Level2::BodyHalfWidth;

    const float minY = getMinWalkableY(window);
    const float maxY = getMaxWalkableY(window);

    const sf::Vector2f target =
        cientifico1.getPosition();

    sf::Vector2f resolved =
        previousPosition;

    // El cientifico puede recorrer todo el escenario en X.
    const float minimumX =
        worldBounds.position.x + bodyHalfWidth;

    const float maximumX =
        worldBounds.position.x +
        worldBounds.size.x -
        bodyHalfWidth;

    // Resolver movimiento horizontal.
    sf::Vector2f candidateX(
        std::clamp(
            target.x,
            minimumX,
            maximumX
        ),
        std::clamp(
            resolved.y,
            minY,
            maxY
        )
    );

    if (!collidesAt(candidateX))
    {
        resolved.x = candidateX.x;
    }

    // Resolver movimiento vertical.
    sf::Vector2f candidateY(
        resolved.x,
        std::clamp(
            target.y,
            minY,
            maxY
        )
    );

    if (!collidesAt(candidateY))
    {
        resolved.y = candidateY.y;
    }
    else
    {
        resolved.y = std::clamp(
            resolved.y,
            minY,
            maxY
        );
    }

    cientifico1.setPosition(resolved);
}
