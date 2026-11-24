#pragma once

#include "Bullet.hpp"
#include "EnemyFactory.hpp"
#include "Player.hpp"
#include "levels/LevelState.hpp"
#include <array.hpp>
#include <memory>
#include <optional>

class FutureLevel1 final : public LevelState
{
public:
    FutureLevel1(sf::RenderWindow* window, Game* game);

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

    struct SteamParticle
    {
        sf::Vector2f origin;
        sf::Vector2f offset;
        float velocityX;
        float velocityY;
        float baseRadius;
        float life;
        float maxLife;
        float phase;
    };

    Player player;
    EnemyFactory enemyFactory;
    Designar::DynArray<std::unique_ptr<Enemy>> enemies;
    Designar::DynArray<EnemyEntrance> enteringEnemies;
    Designar::DynArray<Bullet> playerBullets;
    Designar::DynArray<std::unique_ptr<Item>> worldWeapons;
    Designar::DynArray<sf::Vector2f> radioactiveVents;
    Designar::DynArray<SteamParticle> steamParticles;

    sf::FloatRect worldBounds;
    float steamAnimTime = 0.f;
    float steamDamageTimer = 0.f;
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
    void updateRadioactiveVents(float deltaTime);
    void spawnEnemyDrop(const sf::Vector2f& position);
    void drawRadioactiveVents();
    void drawAmmoCounter(int ammo);
};