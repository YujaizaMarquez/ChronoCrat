#include "Serpent.hpp"

#include <stdexcept>
#include <utility>

Serpent::Serpent()
    : Enemy()
{
    if (!loadSpriteSheet(
            GameConfig::Level2::SerpentAttackSpritePath,
            70,
            70))
    {
        throw std::runtime_error(
            "No se pudo cargar la animacion de serpiente_murcielago");
    }

    addAnimationClip("idle", 0, 1, 0.1f, true);
    addAnimationClip("walk", 0, 1, 0.1f, true);
    addAnimationClip("attack", 0, 5, 0.1f, true);
    setVisualScale(GameConfig::Level2::SerpentVisualScale);
    setFacingRight(false);
    playAnimation("idle", true);
    setCombatStats(
        GameConfig::Level2::SerpentDetectionRange,
        GameConfig::Level2::SerpentAttackRange,
        GameConfig::Level2::SerpentAttackDamage,
        GameConfig::Level2::SerpentAttackCooldown,
        GameConfig::Enemy::ExperienceReward);
}

void Serpent::buildBehaviorTree()
{
    auto root = std::make_unique<Selector>();

    auto deathSequence = std::make_unique<Sequence>();
    deathSequence->addChild(std::make_unique<ConditionNode>(
        [](BTContext& context)
        {
            return context.enemy.getHp() <= 0;
        }));
    deathSequence->addChild(std::make_unique<ActionNode>(
        [this](BTContext& context)
        {
            return stepDeath(context.dt, context.player);
        }));

    auto attackSequence = std::make_unique<Sequence>();
    attackSequence->addChild(std::make_unique<ConditionNode>(
        [](BTContext& context)
        {
            return context.player.getHp() > 0 &&
                   context.distanceToPlayer <=
                       context.enemy.getAttackRange();
        }));
    attackSequence->addChild(std::make_unique<ActionNode>(
        [this](BTContext& context)
        {
            updateAttack(context.dt, context.player);
            return NodeStatus::Running;
        }));

    auto chaseSequence = std::make_unique<Sequence>();
    chaseSequence->addChild(std::make_unique<ConditionNode>(
        [](BTContext& context)
        {
            return context.player.getHp() > 0 &&
                   context.distanceToPlayer <=
                       context.enemy.getDetectionRange();
        }));
    chaseSequence->addChild(std::make_unique<ActionNode>(
        [this](BTContext& context)
        {
            updateChase(context.dt, context.player);
            return NodeStatus::Running;
        }));

    auto idleAction = std::make_unique<ActionNode>(
        [this](BTContext& context)
        {
            return stepIdle(context.dt);
        });

    root->addChild(std::move(deathSequence));
    root->addChild(std::move(attackSequence));
    root->addChild(std::move(chaseSequence));
    root->addChild(std::move(idleAction));
    behaviorTree = std::move(root);
}

void Serpent::updateChase(float deltaTime, const Player& player)
{
    setFacingRight(player.getPosition().x >= getPosition().x);
    Enemy::updateChase(deltaTime, player);
}

void Serpent::updateAttack(float deltaTime, Player& player)
{
    setFacingRight(player.getPosition().x >= getPosition().x);
    move({0.f, 0.f}, deltaTime);
    playAnimation("attack");

    if (attackTimer <= 0.f)
    {
        player.setHp(player.getHp() - attackDamage);
        attackTimer = attackCooldown;
    }
}
