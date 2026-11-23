#pragma once

#include "Bullet.hpp"
#include "Enemy.hpp"
#include "GameConfig.hpp"
#include <array.hpp>

class Robot final : public Enemy
{
public:
    Robot();

    void setCollisionGeometry(
        const Designar::DynArray<sf::FloatRect>& projectileAreas,
        const sf::FloatRect& bounds);
    void draw(sf::RenderWindow& window) override;

protected:
    void buildBehaviorTree() override;
    void updateAdditionalSystems(float deltaTime, Player& player) override;

private:
    const Designar::DynArray<sf::FloatRect>* projectileCollisionAreas = nullptr;
    sf::FloatRect worldBounds;
    Designar::DynArray<Bullet> lasers;

    float meleeRange = GameConfig::Robot::MeleeRange;
    float meleeChaseRange = GameConfig::Robot::MeleeChaseRange;
    float laserRange = GameConfig::Robot::LaserRange;
    float laserCooldown = GameConfig::Robot::LaserCooldown;
    float laserTimer = 0.f;
    float laserSpeed = GameConfig::Robot::LaserSpeed;
    int laserDamage = GameConfig::Robot::LaserDamage;
    float laneTolerance = GameConfig::Robot::LaneTolerance;

    static constexpr float muzzleOffset = GameConfig::Robot::MuzzleOffset;
    static constexpr float muzzleHeight = GameConfig::Robot::MuzzleHeight;

    bool isAlignedForLaser(const Player& player) const;
    bool hasLineOfSight(const sf::Vector2f& target) const;
    NodeStatus stepLaserAttack(float deltaTime, const Player& player);
    NodeStatus stepPositionForLaser(float deltaTime, const Player& player);
};