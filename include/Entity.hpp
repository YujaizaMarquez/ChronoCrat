#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Animation.hpp"
#include "json.hpp"
#include "Savemanager.hpp"

class Entity {
private:
    std::string id = "heroe";
    std::string name = "Heroe";
    int hp = 250;
    int maxHp = 250;
    int attackDamage = 25;
    float speed = 180.f;

    sf::Vector2f position = {200.f, 500.f};
    sf::Texture texture;
    sf::Sprite sprite;
    Animation anim;

public:
    Entity();
    virtual ~Entity() = default;

    bool loadFromJson(const std::string& jsonPath);
    void move(const sf::Vector2f& direction, float dt);
    void playAnimation(const std::string& clipName, bool forceRestart = false);

    virtual void update(float dt);
    virtual void draw(sf::RenderWindow& window);

    void setPosition(const sf::Vector2f& pos);
    const sf::Vector2f& getPosition() const;

    void setHp(int newHp);
    int getHp() const;
    int getMaxHp() const;
    const std::string& getId() const;
    SaveData exportSaveData(const std::string& currentEra, int levelId) const;
    void importSaveData(const SaveData& data);
};