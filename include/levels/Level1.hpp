#pragma once

#include "Bullet.hpp"
#include "EnemyFactory.hpp"
#include "Player.hpp"
#include "levels/LevelState.hpp"
#include <array.hpp>
#include <memory>
#include <optional>

class Level1 final : public LevelState
{
public:
    Level1(sf::RenderWindow* window, Game* game);

protected:
    const Player& getPlayer() const override;
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

    Player player;
    EnemyFactory enemyFactory;
    Designar::DynArray<std::unique_ptr<Enemy>> enemies;
    Designar::DynArray<EnemyEntrance> enteringEnemies;
    Designar::DynArray<std::unique_ptr<Item>> worldWeapons;
    Designar::DynArray<sf::FloatRect> movementCollisionAreas;
    Designar::DynArray<sf::FloatRect> projectileCollisionAreas;
    Designar::DynArray<Bullet> playerBullets;
    sf::FloatRect worldBounds;
    int activeEncounterIndex = -1;
    bool levelCompleted = false;
    bool playerDefeated = false;

    void initializeCollisionAreas();
    bool collidesAt(const sf::Vector2f& position) const;
    void resolveMovement(
        Entity& entity,
        const sf::Vector2f& previousPosition,
        const std::optional<LevelArenaBounds>& activeArenaBounds);
    void updateWorldWeapon(float deltaTime);
    void updatePlayerWeapon(float deltaTime);
    void updatePlayerBullets(float deltaTime);
    void drawAmmoCounter(int ammo);
};