#include "Player.hpp"
#include <cmath>
#include <iostream>

Player::Player() : Entity(), playerLevel(1), exp(0)
{
    if (!loadFromJson("data/heroe.json")) {
        std::cerr << "[ERROR] No se pudo cargar data/heroe.json en Player\n";
    }
}

void Player::handleInput(float deltaTime)
{
    sf::Vector2f dir(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dir.x += 1.f;

    // Normalizar el vector para que no corra más rápido en diagonal
    if (dir.x != 0.f && dir.y != 0.f) {
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        dir /= length;
    }

    move(dir, deltaTime);
}

void Player::update(float deltaTime)
{
    handleInput(deltaTime);
    Entity::update(deltaTime);
}

void Player::addExp(int amount)
{
    if (amount <= 0) return;
    exp += amount;

    // Sube de nivel cada 100 puntos de experiencia
    while (exp >= 100) {
        exp -= 100;
        ++playerLevel;
    }
}

int Player::getPlayerLevel() const
{
    return playerLevel;
}

int Player::getExp() const
{
    return exp;
}

void Player::addItem(const std::string& item)
{
    if (!hasItem(item)) {
        inventory.append(item);
    }
}

bool Player::hasItem(const std::string& item) const
{
    for (size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i] == item) {
            return true;
        }
    }
    return false;
}

void Player::clearInventory()
{
    inventory.clear();
}

const Designar::DynArray<std::string>& Player::getInventory() const
{
    return inventory;
}

void Player::addDefeatedBoss(const std::string& bossId)
{
    if (!hasDefeatedBoss(bossId)) {
        defeatedBosses.append(bossId);
    }
}

bool Player::hasDefeatedBoss(const std::string& bossId) const
{
    for (size_t i = 0; i < defeatedBosses.size(); ++i) {
        if (defeatedBosses[i] == bossId) {
            return true;
        }
    }
    return false;
}

const Designar::DynArray<std::string>& Player::getDefeatedBosses() const
{
    return defeatedBosses;
}

SaveData Player::exportSaveData(const std::string& currentEra, int levelId) const
{
    SaveData data = Entity::exportSaveData(currentEra, levelId);
    data.playerLevel = playerLevel;
    data.exp = exp;

    data.inventory.clear();
    for (size_t i = 0; i < inventory.size(); ++i) {
        data.inventory.append(inventory[i]);
    }

    data.defeatedBosses.clear();
    for (size_t i = 0; i < defeatedBosses.size(); ++i) {
        data.defeatedBosses.append(defeatedBosses[i]);
    }

    return data;
}

void Player::importSaveData(const SaveData& data)
{
    Entity::importSaveData(data);
    playerLevel = data.playerLevel;
    exp = data.exp;

    inventory.clear();
    for (size_t i = 0; i < data.inventory.size(); ++i) {
        inventory.append(data.inventory[i]);
    }

    defeatedBosses.clear();
    for (size_t i = 0; i < data.defeatedBosses.size(); ++i) {
        defeatedBosses.append(data.defeatedBosses[i]);
    }
}