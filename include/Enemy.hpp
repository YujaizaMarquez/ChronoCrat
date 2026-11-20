#pragma once
#include "BehaviorTree.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include <array.hpp>
#include <memory>
#include <string>

class Enemy : public Entity
{
protected:
    std::unique_ptr<BTNode> behaviorTree;
    Designar::DynArray<sf::Vector2f> patrolPoints;
    size_t currentWaypointIndex;

    float detectionRange;
    float attackRange;
    int attackDamage;
    float attackCooldown;
    float attackTimer;
    float idleWaitTime;
    float idleTimer;
    bool waitingAtWaypoint;

    int expReward;
    bool isBoss;
    bool rewardGranted;
    bool dead;

    // Las subclases pueden sobrescribir este método para armar su propio árbol
    virtual void buildBehaviorTree();

    // Tareas base reutilizables por los nodos hoja del árbol
    NodeStatus stepDeath(float dt, Player& player);
    NodeStatus stepAttack(float dt, Player& player);
    NodeStatus stepChase(float dt, const Player& player);
    NodeStatus stepPatrol(float dt);
    NodeStatus stepIdle(float dt);

public:
    Enemy();
    Enemy(const std::string& jsonPath, bool boss = false);
    ~Enemy() override = default;

    void addPatrolPoint(const sf::Vector2f& point);
    void clearPatrolPoints();
    void setCombatStats(float detectRad, float attackRad, int damage, float cooldown, int exp);
    void setIsBoss(bool boss);

    float getDetectionRange() const;
    float getAttackRange() const;
    bool hasPatrolPoints() const;

    void updateAI(float dt, Player& player);
    void update(float dt) override;

    bool isDead() const;

protected:
    virtual void updateChase(float dt, const Player& player);
    virtual void updateAttack(float dt, Player& player);
};