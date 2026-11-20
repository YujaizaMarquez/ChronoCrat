#include "Enemy.hpp"
#include <cmath>
#include <iostream>

Enemy::Enemy()
    : Entity(),
      currentState(EnemyState::Idle),
      currentWaypointIndex(0),
      detectionRange(250.f),
      attackRange(45.f),
      attackDamage(15),
      attackCooldown(1.2f),
      attackTimer(0.f),
      idleWaitTime(1.5f),
      idleTimer(0.f),
      expReward(50),
      isBoss(false),
      rewardGranted(false)
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

void Enemy::addPatrolPoint(const sf::Vector2f& point)
{
    patrolPoints.append(point);
    if (currentState == EnemyState::Idle && patrolPoints.size() > 0) {
        currentState = EnemyState::Patrol;
    }
}

void Enemy::clearPatrolPoints()
{
    patrolPoints.clear();
    currentWaypointIndex = 0;
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

void Enemy::updateAI(float dt, Player& player)
{
    sf::Vector2f toPlayer = player.getPosition() - getPosition();
    float distanceToPlayer = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

    if (getHp() > 0 && player.isAttacking() && distanceToPlayer <= player.getAttackRange()) {
        setHp(getHp() - player.getAttackDamage());}

    if (getHp() <= 0) {
        currentState = EnemyState::Dead;
        move({0.f, 0.f}, dt);
        if (!rewardGranted) {
            player.addExp(expReward);
            if (isBoss && !getId().empty()) {
                player.addDefeatedBoss(getId());
            }
            rewardGranted = true;
            std::cout << "[IA] Enemigo derrotado (" << getId() << "). +" << expReward << " EXP.\n";
        }
        
        update(dt);
        return;
    }

    // Actualizar enfriamiento de ataque
    if (attackTimer > 0.f) {
        attackTimer -= dt;
    }
    // Transiciones de estado según la distancia y vida del jugador
    if (player.getHp() > 0) {
        if (distanceToPlayer <= attackRange) {
            currentState = EnemyState::Attack;
        } else if (distanceToPlayer <= detectionRange) {
            currentState = EnemyState::Chase;
        } else if (patrolPoints.size() > 0 && currentState != EnemyState::Idle) {
            currentState = EnemyState::Patrol;
        } else if (patrolPoints.size() == 0) {
            currentState = EnemyState::Idle;
        }
    } else {
        currentState = (patrolPoints.size() > 0) ? EnemyState::Patrol : EnemyState::Idle;
    }

    // 4. Ejecutar comportamiento del estado activo
    switch (currentState) {
        case EnemyState::Idle:
            move({0.f, 0.f}, dt);
            if (patrolPoints.size() > 0) {
                idleTimer += dt;
                if (idleTimer >= idleWaitTime) {
                    idleTimer = 0.f;
                    currentState = EnemyState::Patrol;
                }
            }
            break;

        case EnemyState::Patrol:
            updatePatrol(dt);
            break;

        case EnemyState::Chase:
            updateChase(dt, player);
            break;

        case EnemyState::Attack:
            updateAttack(dt, player);
            break;

        case EnemyState::Dead:
            move({0.f, 0.f}, dt);
            break;
    }

   
    update(dt);
}

void Enemy::updatePatrol(float dt)
{
    if (patrolPoints.size() == 0) {
        currentState = EnemyState::Idle;
        return;
    }

    sf::Vector2f target = patrolPoints[currentWaypointIndex];
    sf::Vector2f diff = target - getPosition();
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    // Si llegó al punto de control (margen de 5 píxeles), pasa al siguiente
    if (dist <= 5.f) {
        currentWaypointIndex = (currentWaypointIndex + 1) % patrolPoints.size();
        currentState = EnemyState::Idle; // Pequeña pausa al llegar al punto
        move({0.f, 0.f}, dt);
        return;
    }

    sf::Vector2f dir = diff / dist;
    move(dir, dt);
}

void Enemy::updateChase(float dt, const Player& player)
{
    sf::Vector2f diff = player.getPosition() - getPosition();
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (dist > 0.001f) {
        sf::Vector2f dir = diff / dist;
        move(dir, dt);
    }
}

void Enemy::updateAttack(float dt, Player& player)
{
    // Detener el desplazamiento mientras golpea
    move({0.f, 0.f}, dt);

    if (attackTimer <= 0.f) {
        player.setHp(player.getHp() - attackDamage);
        attackTimer = attackCooldown;
        std::cout << "[IA] " << getId() << " ataco al jugador (-" << attackDamage 
                  << " HP). Vida restante: " << player.getHp() << "/" << player.getMaxHp() << "\n";
    }
}

void Enemy::update(float dt)
{
    Entity::update(dt);
}

EnemyState Enemy::getState() const
{
    return currentState;
}

bool Enemy::isDead() const
{
    return currentState == EnemyState::Dead;
}