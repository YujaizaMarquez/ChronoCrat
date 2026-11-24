#pragma once
#include "GameConfig.hpp"
#include "Animation.hpp"
#include <string>

enum class ItemType
{
    None,
    HealthPotion,
    StaminaPotion,
    Weapon
};

class Item
{
protected:
    float durability;
    float damage;
    ItemType type;

private:
    sf::Texture texture;
    sf::Sprite sprite;
    Animation animation;
    sf::Vector2f position = {0.f, 0.f};
    sf::Vector2f groundPosition = {0.f, 0.f};
    float elapsedTime = 0.f;
    bool textureLoaded = false;
    bool beingPickedUp = false;
    bool collected = false;

public:
    Item(
        ItemType type,
        float durability,
        float damage,
        int frameWidth = GameConfig::Animation::DefaultFrameWidth,
        int frameHeight = GameConfig::Animation::DefaultFrameHeight);
    bool loadSpriteSheet(const std::string& path, bool maskBlackBackground = false);
    bool loadFromImage(const sf::Image& image);
    ItemType getType() const;
    float getDurability() const;
    float getDamage() const;
    void setVisualScale(float scale);
    Animation& getAnimation();
    const Animation& getAnimation() const;
    void setPosition(const sf::Vector2f& position);
    const sf::Vector2f& getPosition() const;
    bool tryPickup(
        const sf::Vector2f& playerPosition,
        float pickupRadius = GameConfig::Item::PickupRadius);
    void update(float deltaTime, const sf::Vector2f& playerPosition);
    void draw(sf::RenderWindow& window) const;
    void drawHeld(sf::RenderWindow& window) const;
    void drawInventoryIcon(
        sf::RenderWindow& window,
        const sf::FloatRect& bounds) const;
    bool isBeingPickedUp() const;
    bool isCollected() const;
    virtual void use() = 0;
    virtual ~Item() = default;
};