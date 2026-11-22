#pragma once

#include "Bullet.hpp"
#include "EnemyFactory.hpp"
#include "Player.hpp"
#include "levels/LevelState.hpp"
#include <array.hpp>
#include <memory>
#include <optional>

class Level2 final : public LevelState
{
public:
    Level2(sf::RenderWindow* window, Game* game);

protected:
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
    Player player;
    EnemyFactory enemyFactory;
    Designar::DynArray<std::unique_ptr<Enemy>> enemies;
    Designar::DynArray<sf::FloatRect> collisionAreas;
    Designar::DynArray<Bullet> playerBullets;
    std::unique_ptr<Item> worldWeapon;
    sf::FloatRect worldBounds;

    int activeEncounterIndex = -1;
    bool levelCompleted = false;
    bool playerDefeated = false;

    void initializeCollisionAreas();
    bool collidesAt(const sf::Vector2f& position) const;
    void resolveMovement(Entity& entity, const sf::Vector2f& previousPosition);
    void updateWorldWeapon(float deltaTime);
    void updatePlayerWeapon(float deltaTime);
    void updatePlayerBullets(float deltaTime);
    void drawAmmoCounter(int ammo);
};