#pragma once
#include "Entity.hpp"
#include "item/Inventory.hpp"
#include <array.hpp>
#include <memory>
#include <string>

class Weapon;

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

    Inventory inventory;
    Designar::DynArray<std::string> savedInventoryIds;
    Designar::DynArray<std::string> defeatedBosses;

public:
    Player();
    ~Player() override = default;
    
    void handleInput(float deltaTime);
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
    void addItem(const std::string& item);
    bool hasItem(ItemType type) const;
    bool hasItem(const std::string& item) const;
    void clearInventory();
    Inventory& getInventory();
    const Inventory& getInventory() const;
    Weapon* getEquippedWeapon();
    const Weapon* getEquippedWeapon() const;
    const Designar::DynArray<std::string>& getSavedInventoryIds() const;

    void addDefeatedBoss(const std::string& bossId);
    bool hasDefeatedBoss(const std::string& bossId) const;
    const Designar::DynArray<std::string>& getDefeatedBosses() const;

    SaveData exportSaveData(const std::string& currentEra, int levelId) const;
    void importSaveData(const SaveData& data);
};