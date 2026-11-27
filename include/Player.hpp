#pragma once
#include "Entity.hpp"
#include "GameConfig.hpp"
#include "item/Inventory.hpp"
#include <array.hpp>
#include <memory>
#include <string>

class Weapon;

struct PlayerUpgradeBonuses
{
    int bonusMaxHp = 0;
    float bonusSpeed = 0.f;
};

class Player : public Entity
{
private:
    int playerLevel;
    int exp;

    //Para los coñazos
    int attackDamage;
    float attackRange;
    float attackCooldown;
    float attackTimer;
    bool attackingThisFrame;
    bool attackKeyWasPressed = false;
    int attackFrameIndex = -1;
    bool attackAnimationActive = false;
    bool deathAnimationStarted = false;
    bool spriteAnimationsConfigured = false;
    bool wearingMask = false;
    std::string walkSpriteSheet;
    std::string maskedWalkSpriteSheet;
    std::string attackSpriteSheet;
    std::string deathSpriteSheet;

    static constexpr int AttackFrameCount = 5;

    Inventory inventory;
    std::size_t selectedInventorySlot = 0;
    Designar::DynArray<std::string> savedInventoryIds;
    Designar::DynArray<std::string> defeatedBosses;

    void updateEquippedInventoryItem();
    static PlayerUpgradeBonuses activeUpgradeBonuses;

public:
    Player();
    ~Player() override = default;
    
    void handleInput(float deltaTime);
    void configureSpriteAnimations(
        const std::string& walkPath,
        const std::string& maskedWalkPath,
        const std::string& attackPath,
        const std::string& deathPath);
    void setWearingMask(bool wearing);
    void update(float deltaTime) override;
    void update(float deltaTime, bool inputEnabled);

    bool isDead() const;
    bool isAttacking() const;
    int getAttackDamage() const;
    float getAttackRange() const;
    void setCombatStats(int damage, float range, float cooldown);
    bool attackTarget(Entity& target);

    void addExp(int amount);
    int getPlayerLevel() const;
    int getExp() const;

    bool addItem(std::unique_ptr<Item>&& item);
    bool selectInventorySlotFromKey(sf::Keyboard::Key key);
    void selectInventorySlot(std::size_t index);
    std::size_t getSelectedInventorySlot() const;
    void addItem(const std::string& item);
    bool hasItem(ItemType type) const;
    bool hasItem(const std::string& item) const;
    void clearInventory();
    Inventory& getInventory();
    const Inventory& getInventory() const;
    Weapon* getEquippedWeapon();
    const Weapon* getEquippedWeapon() const;
    bool useMedkit();
    const Designar::DynArray<std::string>& getSavedInventoryIds() const;

    void addDefeatedBoss(const std::string& bossId);
    bool hasDefeatedBoss(const std::string& bossId) const;
    const Designar::DynArray<std::string>& getDefeatedBosses() const;

    SaveData exportSaveData(const std::string& currentEra, int levelId) const;
    void importSaveData(const SaveData& data);
    static void setActiveUpgradeBonuses(const PlayerUpgradeBonuses& bonuses);
    static const PlayerUpgradeBonuses& getActiveUpgradeBonuses();
};