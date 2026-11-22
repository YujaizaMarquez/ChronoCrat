#include "Bullet.hpp"
#include <algorithm>
#include <cmath>

Bullet::Bullet(
    const sf::Vector2f& position,
    float horizontalDirection,
    float speed,
    int damage,
    sf::Color color)
    : horizontalSpeed((horizontalDirection < 0.f ? -1.f : 1.f) * std::abs(speed)),
      damage(damage)
{
    shape.setOrigin({GameConfig::Bullet::Radius, GameConfig::Bullet::Radius});
    shape.setPosition(position);
    shape.setFillColor(color);
}

bool Bullet::update(
    float deltaTime,
    Entity& target,
    const Designar::DynArray<sf::FloatRect>& collisionAreas,
    const sf::FloatRect& worldBounds)
{
    Designar::DynArray<Entity*> targets;
    targets.append(&target);
    return update(deltaTime, targets, collisionAreas, worldBounds);
}

bool Bullet::update(
    float deltaTime,
    const Designar::DynArray<Entity*>& targets,
    const Designar::DynArray<sf::FloatRect>& collisionAreas,
    const sf::FloatRect& worldBounds)
{
    const float dt = std::max(0.f, deltaTime);
    lifetime -= dt;
    if (lifetime <= 0.f)
    {
        return false;
    }

    const sf::Vector2f start = shape.getPosition();
    const float horizontalTravel = horizontalSpeed * dt;
    const int steps = std::max(
        1,
        static_cast<int>(std::ceil(
            std::abs(horizontalTravel) / GameConfig::Bullet::CollisionStep)));

    for (int step = 1; step <= steps; ++step)
    {
        const float fraction = static_cast<float>(step) / steps;
        const sf::Vector2f position(
            start.x + horizontalTravel * fraction,
            start.y);
        const sf::FloatRect bulletBounds(
            {position.x - GameConfig::Bullet::Radius,
             position.y - GameConfig::Bullet::Radius},
            {GameConfig::Bullet::Radius * 2.f,
             GameConfig::Bullet::Radius * 2.f});

        if (position.x < worldBounds.position.x ||
            position.x > worldBounds.position.x + worldBounds.size.x ||
            position.y < worldBounds.position.y ||
            position.y > worldBounds.position.y + worldBounds.size.y)
        {
            return false;
        }

        for (std::size_t areaIndex = 0; areaIndex < collisionAreas.size(); ++areaIndex)
        {
            const sf::FloatRect& area = collisionAreas[areaIndex];
            if (area.findIntersection(bulletBounds))
            {
                return false;
            }
        }

        for (std::size_t targetIndex = 0; targetIndex < targets.size(); ++targetIndex)
        {
            Entity* target = targets[targetIndex];
            if (target == nullptr || target->getHp() <= 0)
            {
                continue;
            }

            const sf::Vector2f targetPosition = target->getPosition();
            const sf::FloatRect targetBounds(
                {targetPosition.x - GameConfig::Bullet::TargetHalfWidth,
                 targetPosition.y - GameConfig::Bullet::TargetHeight},
                {GameConfig::Bullet::TargetHalfWidth * 2.f,
                 GameConfig::Bullet::TargetHeight});
            if (targetBounds.findIntersection(bulletBounds))
            {
                target->setHp(target->getHp() - damage);
                return false;
            }
        }
    }

    shape.move({horizontalTravel, 0.f});
    return true;
}

void Bullet::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}
