#include "Enemy.hpp"
#include <cmath>
#include <iostream>

Enemy::Enemy()
    : Entity(),
      behaviorTree(nullptr),
      currentWaypointIndex(0),
      detectionRange(250.f),
      attackRange(45.f),
      attackDamage(15),
      attackCooldown(1.2f),
      attackTimer(0.f),
      idleWaitTime(1.5f),
      idleTimer(0.f),
      waitingAtWaypoint(false),
      expReward(50),
      isBoss(false),
      rewardGranted(false),
      dead(false)
{
}

Enemy::Enemy(const std::string& jsonPath, bool boss)
    : Enemy()
{
    isBoss = boss;
    if (!loadFromJson(jsonPath)) {
        std::cerr << "[ERROR] No se pudo cargar el JSON del enemigo: " << jsonPath << "\n";
    }
}

void Enemy::buildBehaviorTree()
{
    auto root = std::make_unique<Selector>();

    // 1. Rama de Muerte (máxima prioridad)
    auto deathSeq = std::make_unique<Sequence>();
    deathSeq->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.enemy.getHp() <= 0;
    }));
    deathSeq->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepDeath(ctx.dt, ctx.player);
    }));

    // 2. Rama de Ataque
    auto attackSeq = std::make_unique<Sequence>();
    attackSeq->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.player.getHp() > 0 && ctx.distanceToPlayer <= ctx.enemy.getAttackRange();
    }));
    attackSeq->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepAttack(ctx.dt, ctx.player);
    }));

    // 3. Rama de Persecución
    auto chaseSeq = std::make_unique<Sequence>();
    chaseSeq->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.player.getHp() > 0 && ctx.distanceToPlayer <= ctx.enemy.getDetectionRange();
    }));
    chaseSeq->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepChase(ctx.dt, ctx.player);
    }));

    // 4. Rama de Patrullaje
    auto patrolSeq = std::make_unique<Sequence>();
    patrolSeq->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.enemy.hasPatrolPoints();
    }));
    patrolSeq->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepPatrol(ctx.dt);
    }));

    // 5. Rama de Reposo por defecto
    auto idleAction = std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepIdle(ctx.dt);
    });

    root->addChild(std::move(deathSeq));
    root->addChild(std::move(attackSeq));
    root->addChild(std::move(chaseSeq));
    root->addChild(std::move(patrolSeq));
    root->addChild(std::move(idleAction));

    behaviorTree = std::move(root);
}

void Enemy::addPatrolPoint(const sf::Vector2f& point)
{
    patrolPoints.append(point);
}

void Enemy::clearPatrolPoints()
{
    patrolPoints.clear();
    currentWaypointIndex = 0;
    waitingAtWaypoint = false;
    idleTimer = 0.f;
}

void Enemy::setCombatStats(float detectRad, float attackRad, int damage, float cooldown, int exp)
{
    detectionRange = detectRad;
    attackRange = attackRad;
    attackDamage = damage;
    attackCooldown = cooldown;
    expReward = exp;
}

void Enemy::setIsBoss(bool boss)
{
    isBoss = boss;
}

float Enemy::getDetectionRange() const
{
    return detectionRange;
}

float Enemy::getAttackRange() const
{
    return attackRange;
}

bool Enemy::hasPatrolPoints() const
{
    return patrolPoints.size() > 0;
}

void Enemy::updateAI(float dt, Player& player)
{
    if (!behaviorTree) {
        buildBehaviorTree();
    }

    sf::Vector2f toPlayer = player.getPosition() - getPosition();
    float distanceToPlayer = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

    // Recibir daño del jugador si está atacando en rango
    if (getHp() > 0 && player.isAttacking() && distanceToPlayer <= player.getAttackRange()) {
        setHp(getHp() - player.getAttackDamage());
    }

    // Actualizar enfriamiento de ataque
    if (attackTimer > 0.f) {
        attackTimer -= dt;
    }

    // Evaluar el Árbol de Comportamientos
    BTContext ctx{*this, player, dt, distanceToPlayer};
    behaviorTree->tick(ctx);

    update(dt);
}

NodeStatus Enemy::stepDeath(float dt, Player& player)
{
    dead = true;
    move({0.f, 0.f}, dt);

    if (!rewardGranted) {
        player.addExp(expReward);
        if (isBoss && !getId().empty()) {
            player.addDefeatedBoss(getId());
        }
        rewardGranted = true;
        std::cout << "[IA] Enemigo derrotado (" << getId() << "). +" << expReward << " EXP.\n";
    }

    return NodeStatus::Success;
}

NodeStatus Enemy::stepAttack(float dt, Player& player)
{
    waitingAtWaypoint = false;
    idleTimer = 0.f;
    move({0.f, 0.f}, dt);

    if (attackTimer <= 0.f) {
        player.setHp(player.getHp() - attackDamage);
        attackTimer = attackCooldown;
        std::cout << "[IA] " << getId() << " ataco al jugador (-" << attackDamage
                  << " HP). Vida restante: " << player.getHp() << "/" << player.getMaxHp() << "\n";
        return NodeStatus::Success;
    }

    return NodeStatus::Running;
}

NodeStatus Enemy::stepChase(float dt, const Player& player)
{
    waitingAtWaypoint = false;
    idleTimer = 0.f;

    sf::Vector2f diff = player.getPosition() - getPosition();
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (dist > 0.001f) {
        sf::Vector2f dir = diff / dist;
        move(dir, dt);
    }

    return NodeStatus::Running;
}

NodeStatus Enemy::stepPatrol(float dt)
{
    if (patrolPoints.size() == 0) {
        return NodeStatus::Failure;
    }

    if (waitingAtWaypoint) {
        move({0.f, 0.f}, dt);
        idleTimer += dt;
        if (idleTimer >= idleWaitTime) {
            idleTimer = 0.f;
            waitingAtWaypoint = false;
        }
        return NodeStatus::Running;
    }

    sf::Vector2f target = patrolPoints[currentWaypointIndex];
    sf::Vector2f diff = target - getPosition();
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (dist <= 5.f) {
        currentWaypointIndex = (currentWaypointIndex + 1) % patrolPoints.size();
        waitingAtWaypoint = true;
        idleTimer = 0.f;
        move({0.f, 0.f}, dt);
        return NodeStatus::Success;
    }

    sf::Vector2f dir = diff / dist;
    move(dir, dt);
    return NodeStatus::Running;
}

NodeStatus Enemy::stepIdle(float dt)
{
    move({0.f, 0.f}, dt);
    return NodeStatus::Success;
}

void Enemy::update(float dt)
{
    Entity::update(dt);
}

bool Enemy::isDead() const
{
    return dead;
}