#include "Entity.hpp"
#include <fstream>
#include <iostream>

Entity::Entity()
    : sprite(texture),
      anim(GameConfig::Animation::DefaultFrameWidth,
           GameConfig::Animation::DefaultFrameHeight) {}

bool Entity::loadFromJson(const std::string& jsonPath) {
    try {
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            std::cerr << "[Entity] No se pudo abrir el archivo: " << jsonPath << "\n";
            return false;
        }

        nlohmann::json j;
        file >> j;

        id = j.value("id", std::string(GameConfig::Entity::DefaultId));
        name = j.value("name", std::string(GameConfig::Entity::DefaultName));
        maxHp = j.value("maxHp", GameConfig::Entity::DefaultHealth);
        hp = maxHp;
        attackDamage = j.value("attack", GameConfig::Entity::DefaultAttackDamage);
        speed = j.value("speed", GameConfig::Entity::DefaultSpeed);

        int fw = j.value("frameWidth", GameConfig::Animation::DefaultFrameWidth);
        int fh = j.value("frameHeight", GameConfig::Animation::DefaultFrameHeight);
        anim = Animation(fw, fh);

        std::string texPath = j.value("spriteSheet", "");
        if (!texPath.empty()) {
            if (!texture.loadFromFile(texPath)) {
                std::cerr << "[Entity] Error al cargar textura: " << texPath << "\n";
                return false;
            }
            sprite.setTexture(texture, true);
            anim.attachSprite(sprite);
        }

        if (j.contains("animations")) {
            anim.loadFromJson(j["animations"]);
            anim.play("idle", true);
        }

        sprite.setPosition(position);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Entity] Error leyendo JSON: " << e.what() << "\n";
        return false;
    }
}

void Entity::move(const sf::Vector2f& direction, float dt) {
    if (direction.x != 0.f || direction.y != 0.f) {
        position.x += direction.x * speed * dt;
        position.y += direction.y * speed * dt;
        sprite.setPosition(position);

        if (direction.x > 0.f) setFacingRight(true);
        else if (direction.x < 0.f) setFacingRight(false);

        anim.play("walk");
    } else {
        anim.play("idle");
    }
}

void Entity::setFacingRight(bool right) {
    facingRight = right;
    anim.setFacingRight(right);
}

bool Entity::isFacingRight() const {
    return facingRight;
}

void Entity::setVisualScale(float scale) {
    sprite.setScale({scale, scale});
}

void Entity::playAnimation(const std::string& clipName, bool forceRestart) {
    anim.play(clipName, forceRestart);
}

void Entity::update(float dt) {
    anim.update(dt);
    sprite.setPosition(position);
}

void Entity::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Entity::setPosition(const sf::Vector2f& pos) {
    position = pos;
    sprite.setPosition(position);
}

const sf::Vector2f& Entity::getPosition() const {
    return position;
}

void Entity::setHp(int newHp) {
    hp = (newHp < 0) ? 0 : (newHp > maxHp ? maxHp : newHp);
}

int Entity::getHp() const {
    return hp;
}

int Entity::getMaxHp() const {
    return maxHp;
}

const std::string& Entity::getId() const {
    return id;
}
SaveData Entity::exportSaveData(const std::string& currentEra, int levelId) const {
    SaveData data;
    data.currentEra = currentEra;
    data.levelId = levelId;
    data.characterId = id;
    data.currentHp = hp;
    data.maxHp = maxHp;
    data.position = position;
    return data;
}

void Entity::importSaveData(const SaveData& data) {
    id = data.characterId;
    maxHp = data.maxHp;
    setHp(data.currentHp);
    setPosition(data.position);
}