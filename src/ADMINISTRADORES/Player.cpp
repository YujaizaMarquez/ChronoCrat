#include "Player.hpp"
#include "item/Weapon.hpp"
#include "item/Medkit.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <utility>

PlayerUpgradeBonuses Player::activeUpgradeBonuses{};

Player::Player() 
    :Entity(), 
    playerLevel(GameConfig::Player::InitialLevel),
    exp(GameConfig::Player::InitialExperience),
    attackDamage(GameConfig::Player::AttackDamage),
    attackRange(GameConfig::Player::AttackRange),
    attackCooldown(GameConfig::Player::AttackCooldown),
    attackTimer(0.f),
    attackingThisFrame(false)
{
    if (!loadFromJson(GameConfig::Player::DataPath)) {
        std::cerr << "[ERROR] No se pudo cargar " << GameConfig::Player::DataPath
                  << " en Player\n";
    }

    if (activeUpgradeBonuses.bonusMaxHp > 0) {
        setMaxHp(getMaxHp() + activeUpgradeBonuses.bonusMaxHp);
        setHp(getMaxHp());
    }

    if (activeUpgradeBonuses.bonusSpeed > 0.f) {
        setSpeed(getSpeed() + activeUpgradeBonuses.bonusSpeed);
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

}

void Player::configureSpriteAnimations(
    const std::string& walkPath,
    const std::string& maskedWalkPath,
    const std::string& attackPath,
    const std::string& deathPath)
{
    walkSpriteSheet = walkPath;
    maskedWalkSpriteSheet = maskedWalkPath;
    attackSpriteSheet = attackPath;
    deathSpriteSheet = deathPath;

    if (!loadSpriteSheet(walkSpriteSheet)) {
        throw std::runtime_error(
            "No se pudo cargar el sprite de caminar del jugador");
    }

    addAnimationClip("idle", 0, 1, 0.12f, true);
    addAnimationClip("walk", 0, 5, 0.12f, true);
    addAnimationClip("attack", 0, AttackFrameCount, 0.07f, false);
    addAnimationClip("die", 0, 5, 0.12f, false);
    spriteAnimationsConfigured = true;
    playAnimation("walk", true);
}

void Player::setWearingMask(bool wearing)
{
    if (wearingMask == wearing) {
        return;
    }

    wearingMask = wearing;
    if (!spriteAnimationsConfigured ||
        attackAnimationActive ||
        isDead()) {
        return;
    }

    const std::string& path = wearingMask
        ? maskedWalkSpriteSheet
        : walkSpriteSheet;

    if (!loadSpriteSheet(path)) {
        throw std::runtime_error(
            "No se pudo cargar el sprite de caminar con el estado actual");
    }
}

void Player::update(float deltaTime)
{
    update(deltaTime, true);
}

void Player::update(float deltaTime, bool inputEnabled)
{
    attackingThisFrame = false;

    if (attackTimer > 0.f) {
        attackTimer -= deltaTime;
    }

    const bool attackKeyPressed =
        inputEnabled &&
        !isDead() &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y);
    const bool attackPressedThisFrame =
        attackKeyPressed && !attackKeyWasPressed;
    attackKeyWasPressed = attackKeyPressed;

    if (attackPressedThisFrame) {
        attackFrameIndex = (attackFrameIndex + 1) % AttackFrameCount;
        if (attackTimer <= 0.f) {
            attackingThisFrame = true;
            attackTimer = attackCooldown;
            std::cout << "[Player] Ataque cuerpo a cuerpo lanzado (Tecla Y)\n";
        }
    }

    if (inputEnabled) {
        handleInput(deltaTime);
    } else {
        move({0.f, 0.f}, deltaTime);
    }

    if (spriteAnimationsConfigured) {
        if (isDead()) {
            if (!deathAnimationStarted) {
                if (!loadSpriteSheet(deathSpriteSheet)) {
                    throw std::runtime_error(
                        "No se pudo cargar el sprite de muerte del jugador");
                }
                playAnimation("die", true);
                deathAnimationStarted = true;
            } else {
                playAnimation("die");
            }
        } else if (attackKeyPressed) {
            if (!attackAnimationActive) {
                if (!loadSpriteSheet(attackSpriteSheet)) {
                    throw std::runtime_error(
                        "No se pudo cargar el sprite de ataque del jugador");
                }
                attackAnimationActive = true;
            }
            playAnimation("attack");
            setAnimationFrame(attackFrameIndex);
        } else if (attackAnimationActive) {
            attackAnimationActive = false;
            const std::string& path = wearingMask
                ? maskedWalkSpriteSheet
                : walkSpriteSheet;
            if (!loadSpriteSheet(path)) {
                throw std::runtime_error(
                    "No se pudo restaurar el sprite de caminar del jugador");
            }
            playAnimation("walk", true);
        }
    }

    Entity::update(deltaTime);

    if (spriteAnimationsConfigured && attackKeyPressed) {
        setAnimationFrame(attackFrameIndex);
    }
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

    while (exp >= GameConfig::Player::ExperiencePerLevel) {
        exp -= GameConfig::Player::ExperiencePerLevel;
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

bool Player::addItem(std::unique_ptr<Item>&& item)
{
    if (!item)
    {
        return false;
    }

    if (!inventory.addItem(std::move(item)))
    {
        return false;
    }

    updateEquippedInventoryItem();
    return true;
}

bool Player::selectInventorySlotFromKey(sf::Keyboard::Key key)
{
    switch (key)
    {
    case sf::Keyboard::Key::Num1:
    case sf::Keyboard::Key::Numpad1:
        selectInventorySlot(0);
        return true;
    case sf::Keyboard::Key::Num2:
    case sf::Keyboard::Key::Numpad2:
        selectInventorySlot(1);
        return true;
    case sf::Keyboard::Key::Num3:
    case sf::Keyboard::Key::Numpad3:
        selectInventorySlot(2);
        return true;
    case sf::Keyboard::Key::Num4:
    case sf::Keyboard::Key::Numpad4:
        selectInventorySlot(3);
        return true;
    case sf::Keyboard::Key::Num5:
    case sf::Keyboard::Key::Numpad5:
        selectInventorySlot(4);
        return true;
    default:
        return false;
    }
}

void Player::selectInventorySlot(std::size_t index)
{
    if (index >= inventory.capacity())
    {
        return;
    }

    selectedInventorySlot = index;
    updateEquippedInventoryItem();
}

std::size_t Player::getSelectedInventorySlot() const
{
    return selectedInventorySlot;
}

void Player::updateEquippedInventoryItem()
{
    for (std::size_t index = 0; index < inventory.capacity(); ++index)
    {
        Weapon* weapon = dynamic_cast<Weapon*>(inventory.get(index));
        if (weapon != nullptr)
        {
            weapon->setEquipped(index == selectedInventorySlot);
        }
    }
}

void Player::addItem(const std::string& item)
{
    if (!hasItem(item)) {
        savedInventoryIds.append(item);
    }
}

bool Player::hasItem(ItemType type) const
{
    return inventory.contains(type);
}

bool Player::hasItem(const std::string& item) const
{
    for (size_t i = 0; i < savedInventoryIds.size(); ++i) {
        if (savedInventoryIds[i] == item) {
            return true;
        }
    }
    return false;
}

void Player::clearInventory()
{
    inventory.clear();
    selectedInventorySlot = 0;
    savedInventoryIds.clear();
}

Inventory& Player::getInventory()
{
    return inventory;
}

const Inventory& Player::getInventory() const
{
    return inventory;
}

Weapon* Player::getEquippedWeapon()
{
    Weapon* weapon =
        dynamic_cast<Weapon*>(inventory.get(selectedInventorySlot));
    return weapon != nullptr && weapon->isEquipped() ? weapon : nullptr;
}

const Weapon* Player::getEquippedWeapon() const
{
    const Weapon* weapon =
        dynamic_cast<const Weapon*>(inventory.get(selectedInventorySlot));
    return weapon != nullptr && weapon->isEquipped() ? weapon : nullptr;
}

bool Player::useMedkit()
{
    if (isDead() || getHp() >= getMaxHp())
    {
        return false;
    }

    std::size_t targetSlot = inventory.capacity();
    if (Item* selected = inventory.get(selectedInventorySlot);
        selected != nullptr && selected->getType() == ItemType::HealthPotion)
    {
        targetSlot = selectedInventorySlot;
    }
    else
    {
        for (std::size_t index = 0; index < inventory.capacity(); ++index)
        {
            Item* item = inventory.get(index);
            if (item != nullptr && item->getType() == ItemType::HealthPotion)
            {
                targetSlot = index;
                break;
            }
        }
    }

    if (targetSlot >= inventory.capacity())
    {
        return false;
    }

    Medkit* medkit = dynamic_cast<Medkit*>(inventory.get(targetSlot));
    const int healAmount = medkit != nullptr
        ? medkit->getHealAmount()
        : GameConfig::Medkit::HealAmount;

    inventory.get(targetSlot)->use();
    setHp(std::min(getMaxHp(), getHp() + healAmount));
    inventory.removeAt(targetSlot);
    updateEquippedInventoryItem();
    return true;
}

const Designar::DynArray<std::string>& Player::getSavedInventoryIds() const
{
    return savedInventoryIds;
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
    for (size_t i = 0; i < savedInventoryIds.size(); ++i) {
        data.inventory.append(savedInventoryIds[i]);
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
    selectedInventorySlot = 0;
    savedInventoryIds.clear();
    for (size_t i = 0; i < data.inventory.size(); ++i) {
        savedInventoryIds.append(data.inventory[i]);
    }

    defeatedBosses.clear();
    for (size_t i = 0; i < data.defeatedBosses.size(); ++i) {
        defeatedBosses.append(data.defeatedBosses[i]);
    }
}

void Player::setActiveUpgradeBonuses(const PlayerUpgradeBonuses& bonuses)
{
    activeUpgradeBonuses = bonuses;
}

const PlayerUpgradeBonuses& Player::getActiveUpgradeBonuses()
{
    return activeUpgradeBonuses;
}