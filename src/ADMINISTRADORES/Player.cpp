#include "Player.hpp"
#include <cmath>
#include <iostream>

Player::Player() 
    :Entity(), 
    playerLevel(1), 
    exp(0),
    attackDamage(35),
    attackRange(65.f),
    attackCooldown(0.45f),
    attackTimer(0.f),
    attackingThisFrame(false)
{
    if (!loadFromJson("data/heroe.json")) {
        std::cerr << "[ERROR] No se pudo cargar data/heroe.json en Player\n";
    }
}

void Player::handleInput(float deltaTime)
{
    if (isDead()) {
        move({0.f, 0.f}, deltaTime);
        return;
    }
    sf::Vector2f dir(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dir.x += 1.f;

    if (dir.x != 0.f && dir.y != 0.f) {
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        dir /= length;
    }

    move(dir, deltaTime);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y) && attackTimer <= 0.f) {
        attackingThisFrame = true;
        attackTimer = attackCooldown;
        std::cout << "[Player] Ataque cuerpo a cuerpo lanzado (Tecla Y)\n";
    }
}

void Player::update(float deltaTime)
{
    attackingThisFrame = false;

    if (attackTimer > 0.f) {
        attackTimer -= deltaTime;
    }

    handleInput(deltaTime);
    Entity::update(deltaTime);
}

bool Player::isDead() const
{
    return getHp() <= 0;
}

bool Player::isAttacking() const
{
    return attackingThisFrame;
}

int Player::getAttackDamage() const
{
    return attackDamage;
}

float Player::getAttackRange() const
{
    return attackRange;
}

void Player::setCombatStats(int damage, float range, float cooldown)
{
    attackDamage = damage;
    attackRange = range;
    attackCooldown = cooldown;
}

bool Player::attackTarget(Entity& target)
{
    if (!attackingThisFrame || target.getHp() <= 0) return false;

    sf::Vector2f diff = target.getPosition() - getPosition();
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (dist <= attackRange) {
        target.setHp(target.getHp() - attackDamage);
        return true;
    }
    return false;
}

void Player::addExp(int amount)
{
    if (amount <= 0) return;
    exp += amount;

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