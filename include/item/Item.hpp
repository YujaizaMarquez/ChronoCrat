#pragma once
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
    Item(ItemType type, float durability, float damage, int frameWidth = 64, int frameHeight = 64);
    bool loadSpriteSheet(const std::string& path);
    ItemType getType() const;
    float getDurability() const;
    float getDamage() const;
    Animation& getAnimation();
    const Animation& getAnimation() const;
    void setPosition(const sf::Vector2f& position);
    const sf::Vector2f& getPosition() const;
    bool tryPickup(const sf::Vector2f& playerPosition, float pickupRadius = 48.f);
    void update(float deltaTime, const sf::Vector2f& playerPosition);
    void draw(sf::RenderWindow& window) const;
    bool isBeingPickedUp() const;
    bool isCollected() const;
    virtual void use() = 0;
    virtual ~Item() = default;
};