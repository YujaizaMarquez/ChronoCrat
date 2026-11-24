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

bool Item::loadSpriteSheet(const std::string& path, bool maskBlackBackground)
{
    if (maskBlackBackground)
    {
        sf::Image image;
        if (!image.loadFromFile(path))
        {
            std::cerr << "[Item] No se pudo cargar la imagen: " << path << '\n';
            return false;
        }

        const sf::Vector2u imageSize = image.getSize();
        for (unsigned int y = 0; y < imageSize.y; ++y)
        {
            for (unsigned int x = 0; x < imageSize.x; ++x)
            {
                const sf::Color pixel = image.getPixel({x, y});
                if (pixel.r <= GameConfig::Item::BlackMaskThreshold &&
                    pixel.g <= GameConfig::Item::BlackMaskThreshold &&
                    pixel.b <= GameConfig::Item::BlackMaskThreshold)
                {
                    image.setPixel({x, y}, sf::Color::Transparent);
                }
            }
        }

        if (!texture.loadFromImage(image))
        {
            std::cerr << "[Item] No se pudo crear la textura: " << path << '\n';
            return false;
        }
    }
    else if (!texture.loadFromFile(path))
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

void Item::setVisualScale(float scale)
{
    sprite.setScale({scale, scale});
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
        if (distance <= GameConfig::Item::PickupArrivalDistance)
        {
            collected = true;
            return;
        }

        const float travelDistance =
            std::min(distance, GameConfig::Item::PickupSpeed * dt);
        position.x += toPlayer.x / distance * travelDistance;
        position.y += toPlayer.y / distance * travelDistance;
        if (distance - travelDistance <= GameConfig::Item::PickupArrivalDistance)
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
    position.y = groundPosition.y +
                 std::sin(elapsedTime * GameConfig::Item::BobFrequency) *
                     GameConfig::Item::BobAmplitude;
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

void Item::drawHeld(sf::RenderWindow& window) const
{
    if (textureLoaded)
    {
        window.draw(sprite);
    }
}

void Item::drawInventoryIcon(
    sf::RenderWindow& window,
    const sf::FloatRect& bounds) const
{
    if (!textureLoaded)
    {
        return;
    }

    sf::Sprite icon(sprite);
    const sf::FloatRect localBounds = icon.getLocalBounds();
    if (localBounds.size.x <= 0.f || localBounds.size.y <= 0.f)
    {
        return;
    }

    const float scale = std::min(
        bounds.size.x / localBounds.size.x,
        bounds.size.y / localBounds.size.y);
    icon.setOrigin({
        localBounds.position.x + localBounds.size.x / 2.f,
        localBounds.position.y + localBounds.size.y / 2.f
    });
    icon.setScale({scale, scale});
    icon.setPosition({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    });
    window.draw(icon);
}

bool Item::isBeingPickedUp() const
{
    return beingPickedUp;
}

bool Item::isCollected() const
{
    return collected;
}