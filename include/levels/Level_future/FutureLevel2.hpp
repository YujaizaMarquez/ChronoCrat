#pragma once

#include "Bullet.hpp"
#include "EnemyFactory.hpp"
#include "Player.hpp"
#include "levels/LevelState.hpp"
#include <array.hpp>
#include <memory>
#include <optional>

class FutureLevel2 final : public LevelState
{
public:
    FutureLevel2(sf::RenderWindow* window, Game* game);

protected:
    Player& getPlayer() override;
    sf::Vector2f getPlayerPosition() const override;
    void handleLevelEvent(sf::Event& event) override;
    void updateLevel(
        float deltaTime,
        bool playerMovementBlocked,
        std::optional<LevelArenaBounds> activeArenaBounds) override;
    void drawWorld() override;
    void drawHud() override;
    void spawnEncounter(int encounterIndex, const LevelEncounter& encounter) override;

private:
    struct EnemyEntrance
    {
        Enemy* enemy;
        sf::Vector2f target;
    };

    struct BulletTracker
    {
        sf::Vector2f position;
        float direction;
        float speed;
        int damage;
    };

    struct ExplosiveBarrel
    {
        sf::Vector2f position;
        int hp;
        bool exploded;
        float hitFlashTimer;
    };

    struct ExplosionEffect
    {
        sf::Vector2f position;
        float timer;
        float duration;
        float maxRadius;
    };

    struct ElectricHazard
    {
        sf::Vector2f position;
        float cycleTime;
        float playerDamageTimer;
        float enemyDamageTimer;
    };

    Player player;
    EnemyFactory enemyFactory;
    Designar::DynArray<std::unique_ptr<Enemy>> enemies;
    Designar::DynArray<EnemyEntrance> enteringEnemies;
    Designar::DynArray<Bullet> playerBullets;
    Designar::DynArray<BulletTracker> playerBulletTrackers;
    Designar::DynArray<std::unique_ptr<Item>> worldWeapons;
    Designar::DynArray<sf::FloatRect> movementObstacles;
    Designar::DynArray<ExplosiveBarrel> explosiveBarrels;
    Designar::DynArray<ExplosionEffect> explosions;
    Designar::DynArray<ElectricHazard> electricHazards;

    sf::FloatRect worldBounds;
    float bleedRemainingTimer = 0.f;
    float bleedTickTimer = 0.f;
    float damageFlashTimer = 0.f;
    sf::Color damageFlashColor{220, 30, 30};
    int previousPlayerHp = 0;

    int activeEncounterIndex = -1;
    bool levelCompleted = false;
    bool playerDefeated = false;

    void clampToWalkableArea(
        Entity& entity,
        const std::optional<LevelArenaBounds>& activeArenaBounds);
    void updateWorldWeapons(float deltaTime);
    void updatePlayerWeapon(float deltaTime);
    void updatePlayerBullets(float deltaTime);
    void detonateBarrel(std::size_t barrelIndex);
    void updateExplosions(float deltaTime);
    void updateElectricHazards(float deltaTime);
    void updateBleedEffect(float deltaTime);
    void spawnEnemyDrop(const sf::Vector2f& position);
    void drawElectricHazards();
    void drawBarricades();
    void drawBarrelsAndExplosions();
    void drawAmmoCounter(int ammo);
    void drawBleedIndicator();
};