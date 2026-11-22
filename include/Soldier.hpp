#pragma once

#include "Bullet.hpp"
#include "Enemy.hpp"
#include "GameConfig.hpp"
#include <cstddef>

class Soldier final : public Enemy
{
public:
    Soldier();

    void setCollisionGeometry(
        const Designar::DynArray<sf::FloatRect>& projectileAreas,
        const sf::FloatRect& worldBounds);
    void setSquadSlot(std::size_t slot, std::size_t squadSize);
    void draw(sf::RenderWindow& window) override;

protected:
    void buildBehaviorTree() override;
    void updateAdditionalSystems(float deltaTime, Player& player) override;
    void updateChase(float deltaTime, const Player& player) override;
    void updateAttack(float deltaTime, Player& player) override;

private:
    const Designar::DynArray<sf::FloatRect>* projectileCollisionAreas = nullptr;
    sf::FloatRect worldBounds;
    Designar::DynArray<Bullet> bullets;
    float shotCooldown = GameConfig::Soldier::ShotCooldown;
    float shotTimer = 0.f;
    float projectileSpeed = GameConfig::Soldier::ProjectileSpeed;
    int projectileDamage = GameConfig::Soldier::ProjectileDamage;
    float laneTolerance = GameConfig::Soldier::LaneTolerance;
    float preferredDistance = GameConfig::Soldier::FarthestShooterDistance;
    float minimumDistance = GameConfig::Soldier::DefaultMinimumDistance;
    float meleeRange = GameConfig::Soldier::MeleeRange;
    sf::Vector2f flankOffset = {0.f, 0.f};
    bool isFlanker = false;
    static constexpr float muzzleOffset = GameConfig::Soldier::MuzzleOffset;
    static constexpr float muzzleHeight = GameConfig::Soldier::MuzzleHeight;

    bool hasLineOfSight(const sf::Vector2f& target) const;
    bool hasReachedFlankPosition(const Player& player) const;
};
