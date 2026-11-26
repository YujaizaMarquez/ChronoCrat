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

    struct MedkitDrop
    {
        sf::Vector2f position;
        float animTime = 0.f;
    };

    struct FireEmber
    {
        sf::Vector2f offset;
        float velocityY;
        float life;
        float maxLife;
    };

    Player player;
    EnemyFactory enemyFactory;
    Designar::DynArray<std::unique_ptr<Enemy>> enemies;
    Designar::DynArray<EnemyEntrance> enteringEnemies;
    Designar::DynArray<Bullet> playerBullets;
    Designar::DynArray<std::unique_ptr<Item>> worldWeapons;
    Designar::DynArray<MedkitDrop> medkits;
    Designar::DynArray<FireEmber> fireEmbers;

    sf::FloatRect worldBounds;
    sf::Vector2f campfirePosition;
    float campfireAnimTime = 0.f;
    float campfireBurnTimer = 0.f;
    float damageFlashTimer = 0.f;
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
    void updateCampfire(float deltaTime);
    void updateMedkits(float deltaTime);
    void spawnEnemyDrop(const sf::Vector2f& position);
    void drawCampfire();
    void drawMedkits();
    void drawAmmoCounter(int ammo);
};