#pragma once
#include "Entity.hpp"
#include "Player.hpp"
#include <array.hpp>
#include <string>

enum class EnemyState {
    Idle,
    Patrol,
    Chase,
    Attack,
    Dead
};

class Enemy : public Entity
{
private:
    EnemyState currentState;
    Designar::DynArray<sf::Vector2f> patrolPoints;
    size_t currentWaypointIndex;

    float detectionRange;
    float attackRange;
    int attackDamage;
    float attackCooldown;
    float attackTimer;
    float idleWaitTime;
    float idleTimer;

    int expReward;
    bool isBoss;
    bool rewardGranted;

    void updatePatrol(float dt);
    void updateChase(float dt, const Player& player);
    void updateAttack(float dt, Player& player);

public:
    Enemy();
    Enemy(const std::string& jsonPath, bool boss = false);
    ~Enemy() override = default;

    void addPatrolPoint(const sf::Vector2f& point);
    void clearPatrolPoints();
    void setCombatStats(float detectRad, float attackRad, int damage, float cooldown, int exp);
    void setIsBoss(bool boss);

    void updateAI(float dt, Player& player);
    void update(float dt) override;

    EnemyState getState() const;
    bool isDead() const;
};