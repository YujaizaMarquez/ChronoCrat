#include "Infected.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <utility>

Infected::Infected(Variant enemyVariant)
    : Enemy(enemyVariant == Variant::Biter
                ? GameConfig::Infected::BiterDataPath
                : GameConfig::Infected::MacheteDataPath),
      variant(enemyVariant)
{
    if (variant == Variant::Biter)
    {
        setCombatStats(
            GameConfig::Infected::BiterDetectionRange,
            GameConfig::Infected::BiterAttackRange,
            GameConfig::Infected::BiterAttackDamage,
            GameConfig::Infected::BiterAttackCooldown,
            GameConfig::Infected::BiterExperienceReward);
    }
    else
    {
        setCombatStats(
            GameConfig::Infected::MacheteDetectionRange,
            GameConfig::Infected::MacheteAttackRange,
            GameConfig::Infected::MacheteAttackDamage,
            GameConfig::Infected::MacheteAttackCooldown,
            GameConfig::Infected::MacheteExperienceReward);
    }
}

Infected::Variant Infected::getVariant() const
{
    return variant;
}

bool Infected::isFrenzied() const
{
    return frenzied;
}

bool Infected::consumeBleedInflicted()
{
    const bool triggered = bleedInflicted;
    bleedInflicted = false;
    return triggered;
}

int Infected::getExperienceReward() const
{
    return expReward;
}

void Infected::buildBehaviorTree()
{
    auto root = std::make_unique<Selector>();

    // 1. Muerte
    auto deathSequence = std::make_unique<Sequence>();
    deathSequence->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.enemy.getHp() <= 0;
    }));
    deathSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepDeath(ctx.dt, ctx.player);
    }));
    root->addChild(std::move(deathSequence));

    // 2. Embestida del machetero (si ya empezó la animación o entra en distancia media)
    if (variant == Variant::Machete)
    {
        auto lungeSequence = std::make_unique<Sequence>();
        lungeSequence->addChild(std::make_unique<ConditionNode>([this](BTContext& ctx) {
            if (ctx.player.getHp() <= 0)
            {
                return false;
            }
            if (lungePhase != LungePhase::None)
            {
                return true;
            }
            return lungeCooldownTimer <= 0.f &&
                   ctx.distanceToPlayer >= GameConfig::Infected::MacheteLungeMinRange &&
                   ctx.distanceToPlayer <= GameConfig::Infected::MacheteLungeMaxRange;
        }));
        lungeSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
            return stepMacheteLunge(ctx.dt, ctx.player);
        }));
        root->addChild(std::move(lungeSequence));
    }

    // 3. Ataque cuerpo a cuerpo (mordida con sangrado o corte de machete)
    auto meleeSequence = std::make_unique<Sequence>();
    meleeSequence->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.player.getHp() > 0 &&
               ctx.distanceToPlayer <= ctx.enemy.getAttackRange();
    }));
    meleeSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        if (variant == Variant::Biter)
        {
            return stepBiteAttack(ctx.dt, ctx.player);
        }
        setFacingRight(ctx.player.getPosition().x >= getPosition().x);
        return stepAttack(ctx.dt, ctx.player);
    }));
    root->addChild(std::move(meleeSequence));

    // 4. Persecución (acelerada si el machetero está en frenesí)
    auto chaseSequence = std::make_unique<Sequence>();
    chaseSequence->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.player.getHp() > 0 &&
               ctx.distanceToPlayer <= ctx.enemy.getDetectionRange();
    }));
    chaseSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepInfectedChase(ctx.dt, ctx.player);
    }));
    root->addChild(std::move(chaseSequence));

    // 5. Patrullaje y reposo
    auto patrolSequence = std::make_unique<Sequence>();
    patrolSequence->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.enemy.hasPatrolPoints();
    }));
    patrolSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepPatrol(ctx.dt);
    }));
    root->addChild(std::move(patrolSequence));

    root->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepIdle(ctx.dt);
    }));

    behaviorTree = std::move(root);
}

void Infected::updateAdditionalSystems(float deltaTime, Player&)
{
    visualPulseTime += deltaTime;
    lungeCooldownTimer = std::max(0.f, lungeCooldownTimer - deltaTime);

    if (getHp() <= 0)
    {
        lungePhase = LungePhase::None;
        return;
    }

    if (variant == Variant::Machete &&
        !frenzied &&
        getMaxHp() > 0 &&
        getHp() * 100 <= getMaxHp() * GameConfig::Infected::MacheteFrenzyThresholdPercent)
    {
        frenzied = true;
        attackDamage = GameConfig::Infected::MacheteFrenzyDamage;
        attackCooldown = GameConfig::Infected::MacheteFrenzyAttackCooldown;
    }
}

NodeStatus Infected::stepBiteAttack(float deltaTime, Player& player)
{
    setFacingRight(player.getPosition().x >= getPosition().x);
    const int hpBefore = player.getHp();
    const NodeStatus status = stepAttack(deltaTime, player);
    if (player.getHp() < hpBefore)
    {
        bleedInflicted = true;
    }
    return status;
}

NodeStatus Infected::stepMacheteLunge(float deltaTime, Player& player)
{
    const sf::Vector2f toPlayer = player.getPosition() - getPosition();
    const float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

    if (lungePhase == LungePhase::None)
    {
        lungePhase = LungePhase::Windup;
        lungePhaseTimer = frenzied
            ? GameConfig::Infected::MacheteLungeFrenzyWindup
            : GameConfig::Infected::MacheteLungeWindup;
        setFacingRight(toPlayer.x >= 0.f);
        move({0.f, 0.f}, deltaTime);
        return NodeStatus::Running;
    }

    if (lungePhase == LungePhase::Windup)
    {
        setFacingRight(toPlayer.x >= 0.f);
        move({0.f, 0.f}, deltaTime);
        lungePhaseTimer -= deltaTime;

        if (lungePhaseTimer <= 0.f)
        {
            lungeDirection = distance > 0.001f
                ? (toPlayer / distance)
                : sf::Vector2f(isFacingRight() ? 1.f : -1.f, 0.f);
            lungePhase = LungePhase::Dash;
            lungePhaseTimer = GameConfig::Infected::MacheteLungeDuration;
            lungeHitApplied = false;
        }
        return NodeStatus::Running;
    }

    const float speedBoost = frenzied
        ? GameConfig::Infected::MacheteLungeFrenzySpeedMultiplier
        : GameConfig::Infected::MacheteLungeSpeedMultiplier;
    move(lungeDirection, deltaTime * speedBoost);

    const sf::Vector2f postDiff = player.getPosition() - getPosition();
    const float postDist = std::sqrt(postDiff.x * postDiff.x + postDiff.y * postDiff.y);
    if (!lungeHitApplied && postDist <= attackRange * 1.15f)
    {
        player.setHp(std::max(0, player.getHp() - attackDamage));
        lungeHitApplied = true;
    }

    lungePhaseTimer -= deltaTime;
    if (lungePhaseTimer <= 0.f)
    {
        lungePhase = LungePhase::None;
        lungeCooldownTimer = frenzied
            ? GameConfig::Infected::MacheteLungeFrenzyCooldown
            : GameConfig::Infected::MacheteLungeCooldown;
        move({0.f, 0.f}, deltaTime);
        return NodeStatus::Success;
    }

    return NodeStatus::Running;
}

NodeStatus Infected::stepInfectedChase(float deltaTime, const Player& player)
{
    if (variant == Variant::Machete && frenzied)
    {
        const sf::Vector2f diff = player.getPosition() - getPosition();
        const float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        setFacingRight(diff.x >= 0.f);
        if (dist > 1.f)
        {
            move(diff / dist, deltaTime * GameConfig::Infected::MacheteFrenzySpeedMultiplier);
        }
        else
        {
            move({0.f, 0.f}, deltaTime);
        }
        return NodeStatus::Running;
    }

    return stepChase(deltaTime, player);
}

void Infected::draw(sf::RenderWindow& window)
{
    // Aro temporal en el suelo para distinguir Mordedor (verde), Machetero (ámbar) y Frenesí (rojo pulsante)
    if (getHp() > 0)
    {
        const float pulse = 0.5f + 0.5f * std::sin(visualPulseTime * (frenzied ? 10.f : 4.f));
        const float radius = variant == Variant::Biter ? 24.f : (frenzied ? 34.f + pulse * 5.f : 30.f);

        sf::CircleShape marker(radius);
        marker.setOrigin({radius, radius});
        marker.setScale({1.f, 0.35f});
        marker.setPosition(getPosition());

        if (variant == Variant::Biter)
        {
            marker.setFillColor(sf::Color(95, 235, 85, 115));
        }
        else if (frenzied)
        {
            marker.setFillColor(sf::Color(
                250,
                45,
                45,
                static_cast<std::uint8_t>(110.f + pulse * 95.f)));
        }
        else
        {
            marker.setFillColor(sf::Color(235, 145, 45, 115));
        }
        window.draw(marker);
    }

    Enemy::draw(window);

    // Aviso visual antes de que el machetero se lance en embestida
    if (lungePhase == LungePhase::Windup && getHp() > 0)
    {
        sf::RectangleShape alertBar({10.f, 24.f});
        alertBar.setOrigin({5.f, 12.f});
        alertBar.setPosition({getPosition().x, getPosition().y - 120.f});
        alertBar.setFillColor(sf::Color(255, 50, 50));
        window.draw(alertBar);
    }
}