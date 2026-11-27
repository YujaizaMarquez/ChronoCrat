#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Animation.hpp"
#include "GameConfig.hpp"
#include "json.hpp"
#include "Savemanager.hpp"

class Entity {
private:
    std::string id = GameConfig::Entity::DefaultId;
    std::string name = GameConfig::Entity::DefaultName;
    int hp = GameConfig::Entity::DefaultHealth;
    int maxHp = GameConfig::Entity::DefaultHealth;
    int attackDamage = GameConfig::Entity::DefaultAttackDamage;
    float speed = GameConfig::Entity::DefaultSpeed;
    bool facingRight = true;

    sf::Vector2f position = {
        GameConfig::Entity::InitialX,
        GameConfig::Entity::InitialY
    };
    sf::Texture texture;
    sf::Sprite sprite;
    Animation anim;

public:
    Entity();
    virtual ~Entity() = default;

    bool loadFromJson(const std::string& jsonPath);
    bool loadSpriteSheet(
        const std::string& texturePath,
        int frameWidth = 0,
        int frameHeight = 0);
    void addAnimationClip(
        const std::string& name,
        int row,
        int frames,
        float duration,
        bool loop);
    bool isAnimationFinished() const;
    void move(const sf::Vector2f& direction, float dt);
    void setVisualScale(float scale);
    void setFacingRight(bool right);
    bool isFacingRight() const;
    void playAnimation(const std::string& clipName, bool forceRestart = false);
    void setAnimationFrame(int frame);

    virtual void update(float dt);
    virtual void draw(sf::RenderWindow& window);

    void setPosition(const sf::Vector2f& pos);
    const sf::Vector2f& getPosition() const;

    void setHp(int newHp);
    int getHp() const;
    int getMaxHp() const;
    void setMaxHp(int newMaxHp);
    void setSpeed(float newSpeed);
    float getSpeed() const;
    const std::string& getId() const;
    SaveData exportSaveData(const std::string& currentEra, int levelId) const;
    void importSaveData(const SaveData& data);
};