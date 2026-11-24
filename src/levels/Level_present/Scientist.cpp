#include "Scientist.hpp"

#include <cmath>

Scientist::Scientist()
    : Enemy("data/cientifico1.json")
{
}

void Scientist::updateChase(
    float deltaTime,
    const Player& player)
{
    const sf::Vector2f difference =
        player.getPosition() - getPosition();

    float directionX = 0.f;

    if (difference.x > 5.f)
    {
        directionX = 1.f;
    }
    else if (difference.x < -5.f)
    {
        directionX = -1.f;
    }

    if (directionX != 0.f)
    {
        setFacingRight(directionX > 0.f);

        move(
            {directionX, 0.f},
            deltaTime
        );
    }
}