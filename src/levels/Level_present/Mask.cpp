#include "levels/Level_present/Mask.hpp"

Mask::Mask()
    : sprite(texture)
{
}

bool Mask::loadFromFile(
    const std::string& path,
    const sf::Vector2f& position,
    const sf::Vector2f& scale)
{
    if (!texture.loadFromFile(path))
    {
        return false;
    }

    texture.setSmooth(false);
    sprite.setTextureRect(
        sf::IntRect({0, 0}, {FrameWidth, FrameHeight}));
    sprite.setPosition(position);
    sprite.setScale(scale);
    return true;
}

void Mask::update(float deltaTime)
{
    animationTimer += deltaTime;
    while (animationTimer >= FrameDuration)
    {
        animationTimer -= FrameDuration;
        currentFrame = (currentFrame + 1) % FrameCount;
        sprite.setTextureRect(
            sf::IntRect(
                {currentFrame * FrameWidth, 0},
                {FrameWidth, FrameHeight}));
    }
}

void Mask::draw(sf::RenderWindow& window) const
{
    window.draw(sprite);
}

sf::Vector2f Mask::getPosition() const
{
    return sprite.getPosition();
}
