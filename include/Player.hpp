#pragma once
#include "Entity.hpp"
#include <array.hpp>
#include <string>

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

    Designar::DynArray<std::string> inventory;
    Designar::DynArray<std::string> defeatedBosses;

public:
    Player();
    ~Player() override = default;
    
    void handleInput(float deltaTime);
    void update(float deltaTime) override;

    bool isDead() const;
    bool isAttacking() const;
    int getAttackDamage() const;
    float getAttackRange() const;
    void setCombatStats(int damage, float range, float cooldown);
    bool attackTarget(Entity& target);

    void addExp(int amount);
    int getPlayerLevel() const;
    int getExp() const;

    void addItem(const std::string& item);
    bool hasItem(const std::string& item) const;
    void clearInventory();
    const Designar::DynArray<std::string>& getInventory() const;

    void addDefeatedBoss(const std::string& bossId);
    bool hasDefeatedBoss(const std::string& bossId) const;
    const Designar::DynArray<std::string>& getDefeatedBosses() const;

    SaveData exportSaveData(const std::string& currentEra, int levelId) const;
    void importSaveData(const SaveData& data);
};