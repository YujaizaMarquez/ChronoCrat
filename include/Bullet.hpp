#pragma once

#include "Entity.hpp"
#include <array.hpp>

class Bullet
{
public:
    Bullet(
        const sf::Vector2f& position,
        float horizontalDirection,
        float speed,
        int damage,
        sf::Color color = sf::Color(235, 75, 55));

    bool update(
        float deltaTime,
        Entity& target,
        const Designar::DynArray<sf::FloatRect>& collisionAreas,
        const sf::FloatRect& worldBounds);
    bool update(
        float deltaTime,
        const Designar::DynArray<Entity*>& targets,
        const Designar::DynArray<sf::FloatRect>& collisionAreas,
        const sf::FloatRect& worldBounds);
    void draw(sf::RenderWindow& window) const;

private:
    sf::CircleShape shape{12.f};
    float horizontalSpeed;
    float lifetime = 3.f;
    int damage;
};
