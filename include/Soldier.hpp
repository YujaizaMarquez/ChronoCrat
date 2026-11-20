#pragma once

#include "Bullet.hpp"
#include "Enemy.hpp"

class Soldier final : public Enemy
{
public:
    Soldier();

    void setCollisionGeometry(
        const Designar::DynArray<sf::FloatRect>& areas,
        const sf::FloatRect& worldBounds);
    void updateAI(float deltaTime, Player& player) override;
    void draw(sf::RenderWindow& window) override;

protected:
    void updateChase(float deltaTime, const Player& player) override;
    void updateAttack(float deltaTime, Player& player) override;

private:
    const Designar::DynArray<sf::FloatRect>* collisionAreas = nullptr;
    sf::FloatRect worldBounds;
    Designar::DynArray<Bullet> bullets;
    float shotCooldown = 0.9f;
    float shotTimer = 0.f;
    float projectileSpeed = 520.f;
    int projectileDamage = 12;
    float laneTolerance = 10.f;
    float preferredDistance = 600.f;
    float minimumDistance = 180.f;
    float meleeRange = 140.f;
    static constexpr float muzzleOffset = 60.f;
    static constexpr float muzzleHeight = 64.f;

    bool hasLineOfSight(const sf::Vector2f& target) const;
};
