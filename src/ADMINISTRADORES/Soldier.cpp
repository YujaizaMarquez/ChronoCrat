#include "Soldier.hpp"
#include <algorithm>
#include <cmath>

Soldier::Soldier()
    : Enemy("data/enemigo.json")
{
    setCombatStats(2000.f, 1200.f, projectileDamage, shotCooldown, 50);
}

void Soldier::setCollisionGeometry(
    const Designar::DynArray<sf::FloatRect>& areas,
    const sf::FloatRect& bounds)
{
    collisionAreas = &areas;
    worldBounds = bounds;
}

void Soldier::updateAI(float deltaTime, Player& player)
{
    shotTimer = std::max(0.f, shotTimer - deltaTime);
    Enemy::updateAI(deltaTime, player);

    static const Designar::DynArray<sf::FloatRect> noCollisionAreas;
    const Designar::DynArray<sf::FloatRect>& areas =
        collisionAreas != nullptr ? *collisionAreas : noCollisionAreas;

    for (std::size_t index = 0; index < bullets.size();)
    {
        if (!bullets[index].update(
                deltaTime,
                player,
                areas,
                worldBounds))
        {
            bullets.remove_pos(index);
        }
        else
        {
            ++index;
        }
    }
}

void Soldier::updateChase(float deltaTime, const Player& player)
{
    const sf::Vector2f difference = player.getPosition() - getPosition();
    setFacingRight(difference.x >= 0.f);

    if (std::abs(difference.y) > laneTolerance)
    {
        move({0.f, difference.y > 0.f ? 1.f : -1.f}, deltaTime);
        return;
    }

    if (std::abs(difference.x) > preferredDistance)
    {
        move({difference.x > 0.f ? 1.f : -1.f, 0.f}, deltaTime);
        return;
    }

    move({0.f, 0.f}, deltaTime);
}

void Soldier::updateAttack(float deltaTime, Player& player)
{
    const sf::Vector2f difference = player.getPosition() - getPosition();
    const float horizontalDistance = std::abs(difference.x);
    const float laneDifference = difference.y;
    const bool playerIsRight = difference.x >= 0.f;
    setFacingRight(playerIsRight);

    const float distanceSquared =
        difference.x * difference.x + difference.y * difference.y;
    if (distanceSquared <= meleeRange * meleeRange)
    {
        Enemy::updateAttack(deltaTime, player);
        return;
    }

    if (std::abs(laneDifference) > laneTolerance)
    {
        move({0.f, laneDifference > 0.f ? 1.f : -1.f}, deltaTime);
        return;
    }

    if (horizontalDistance > preferredDistance)
    {
        move({playerIsRight ? 1.f : -1.f, 0.f}, deltaTime);
        return;
    }

    if (horizontalDistance < minimumDistance)
    {
        move({playerIsRight ? -1.f : 1.f, 0.f}, deltaTime);
        return;
    }

    move({0.f, 0.f}, deltaTime);
    if (shotTimer > 0.f || !hasLineOfSight(player.getPosition()))
    {
        return;
    }

    const float direction = playerIsRight ? 1.f : -1.f;
    bullets.append(Bullet(
        sf::Vector2f(
            getPosition().x + direction * muzzleOffset,
            getPosition().y - muzzleHeight),
        direction,
        projectileSpeed,
        projectileDamage));
    shotTimer = shotCooldown;
}

bool Soldier::hasLineOfSight(const sf::Vector2f& target) const
{
    if (collisionAreas == nullptr)
    {
        return true;
    }

    const float startX = getPosition().x;
    const float endX = target.x;
    const int steps = std::max(
        1,
        static_cast<int>(std::ceil(std::abs(endX - startX) / 4.f)));

    for (int step = 1; step < steps; ++step)
    {
        const float fraction = static_cast<float>(step) / steps;
        const sf::Vector2f point(
            startX + (endX - startX) * fraction,
            getPosition().y - muzzleHeight);
        for (std::size_t index = 0; index < collisionAreas->size(); ++index)
        {
            const sf::FloatRect& area = (*collisionAreas)[index];
            const sf::FloatRect bulletBounds(
                {point.x - 12.f, point.y - 12.f},
                {24.f, 24.f});
            if (area.findIntersection(bulletBounds))
            {
                return false;
            }
        }
    }

    return true;
}

void Soldier::draw(sf::RenderWindow& window)
{
    Enemy::draw(window);
    for (std::size_t index = 0; index < bullets.size(); ++index)
    {
        bullets[index].draw(window);
    }
}
