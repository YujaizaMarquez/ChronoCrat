#include "item/Item.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

Item::Item(ItemType type, float durability, float damage, int frameWidth, int frameHeight)
    : durability(durability),
      damage(damage),
      type(type),
      sprite(texture),
      animation(frameWidth, frameHeight)
{
}

bool Item::loadSpriteSheet(const std::string& path)
{
    if (!texture.loadFromFile(path))
    {
        std::cerr << "[Item] No se pudo cargar el spritesheet: " << path << '\n';
        return false;
    }

    textureLoaded = true;
    animation.attachSprite(sprite);
    sprite.setPosition(position);
    return true;
}

ItemType Item::getType() const
{
    return type;
}

float Item::getDurability() const
{
    return durability;
}

float Item::getDamage() const
{
    return damage;
}

Animation& Item::getAnimation()
{
    return animation;
}

const Animation& Item::getAnimation() const
{
    return animation;
}

void Item::setPosition(const sf::Vector2f& newPosition)
{
    position = newPosition;
    groundPosition = newPosition;
    if (textureLoaded)
    {
        sprite.setPosition(position);
    }
}

const sf::Vector2f& Item::getPosition() const
{
    return position;
}

bool Item::tryPickup(const sf::Vector2f& playerPosition, float pickupRadius)
{
    if (collected || beingPickedUp)
    {
        return false;
    }

    const sf::Vector2f toPlayer = playerPosition - position;
    const float distanceSquared = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y;
    const float radius = std::max(0.f, pickupRadius);
    if (distanceSquared > radius * radius)
    {
        return false;
    }

    beingPickedUp = true;
    return true;
}

void Item::update(float deltaTime, const sf::Vector2f& playerPosition)
{
    if (collected)
    {
        return;
    }

    const float dt = std::max(0.f, deltaTime);
    elapsedTime += dt;
    animation.update(dt);

    const sf::Vector2f toPlayer = playerPosition - position;
    const float distanceSquared = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y;

    if (beingPickedUp)
    {
        const float distance = std::sqrt(distanceSquared);
        if (distance <= 8.f)
        {
            collected = true;
            return;
        }

        const float travelDistance = std::min(distance, 360.f * dt);
        position.x += toPlayer.x / distance * travelDistance;
        position.y += toPlayer.y / distance * travelDistance;
        if (distance - travelDistance <= 8.f)
        {
            position = playerPosition;
            collected = true;
        }
        if (textureLoaded)
        {
            sprite.setPosition(position);
        }
        return;
    }

    position.x = groundPosition.x;
    position.y = groundPosition.y + std::sin(elapsedTime * 3.f) * 4.f;
    if (textureLoaded)
    {
        sprite.setPosition(position);
    }
}

void Item::draw(sf::RenderWindow& window) const
{
    if (textureLoaded && !collected)
    {
        window.draw(sprite);
    }
}

bool Item::isBeingPickedUp() const
{
    return beingPickedUp;
}

bool Item::isCollected() const
{
    return collected;
}