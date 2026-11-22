#include "Soldier.hpp"
#include <algorithm>
#include <cmath>

Soldier::Soldier()
    : Enemy(GameConfig::Enemy::SoldierDataPath)
{
    setCombatStats(
        GameConfig::Soldier::DetectionRange,
        meleeRange,
        projectileDamage,
        shotCooldown,
        GameConfig::Soldier::ExperienceReward);
}

void Soldier::setCollisionGeometry(
    const Designar::DynArray<sf::FloatRect>& projectileAreas,
    const sf::FloatRect& bounds)
{
    projectileCollisionAreas = &projectileAreas;
    worldBounds = bounds;
}

void Soldier::setSquadSlot(std::size_t slot, std::size_t squadSize)
{
    squadSize = std::max<std::size_t>(1, squadSize);
    const std::size_t flankerCount =
        squadSize == 1 ? 0 : (squadSize + 1) / 2;

    isFlanker = false;
    flankOffset = {0.f, 0.f};
    shotTimer = shotCooldown * GameConfig::Soldier::SquadShotStagger *
                static_cast<float>(slot) /
                static_cast<float>(squadSize);

    if (slot < flankerCount)
    {
        const float angle = GameConfig::Soldier::FlankStartAngleRadians +
            GameConfig::Soldier::FullCircleRadians *
                static_cast<float>(slot) /
                static_cast<float>(flankerCount);

        isFlanker = true;
        flankOffset = {
            std::cos(angle) * GameConfig::Soldier::FlankRadius,
            std::sin(angle) * GameConfig::Soldier::FlankRadius
        };
        preferredDistance = meleeRange;
        minimumDistance = 0.f;
        return;
    }

    const std::size_t rangedCount = squadSize - flankerCount;
    const std::size_t rangedIndex = slot - flankerCount;
    preferredDistance = rangedCount == 1
        ? GameConfig::Soldier::SingleShooterDistance
        : GameConfig::Soldier::ClosestShooterDistance +
              (GameConfig::Soldier::FarthestShooterDistance -
               GameConfig::Soldier::ClosestShooterDistance) *
                  static_cast<float>(rangedIndex) /
              static_cast<float>(rangedCount - 1);
    minimumDistance =
        preferredDistance * GameConfig::Soldier::MinimumDistanceRatio;
}

void Soldier::buildBehaviorTree()
{
    auto root = std::make_unique<Selector>();

    auto deathSequence = std::make_unique<Sequence>();
    deathSequence->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.enemy.getHp() <= 0;
    }));
    deathSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepDeath(ctx.dt, ctx.player);
    }));

    auto meleeSequence = std::make_unique<Sequence>();
    meleeSequence->addChild(std::make_unique<ConditionNode>([this](BTContext& ctx) {
        return ctx.player.getHp() > 0 &&
               ctx.distanceToPlayer <= ctx.enemy.getAttackRange() &&
               (!isFlanker || hasReachedFlankPosition(ctx.player));
    }));
    meleeSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        Enemy::updateAttack(ctx.dt, ctx.player);
        return NodeStatus::Running;
    }));

    auto approachSequence = std::make_unique<Sequence>();
    approachSequence->addChild(std::make_unique<ConditionNode>([this](BTContext& ctx) {
        return ctx.player.getHp() > 0 &&
               ctx.distanceToPlayer <= ctx.enemy.getDetectionRange() &&
               (isFlanker
                    ? !hasReachedFlankPosition(ctx.player)
                    : ctx.distanceToPlayer > preferredDistance);
    }));
    approachSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        updateChase(ctx.dt, ctx.player);
        return NodeStatus::Running;
    }));

    auto rangedSequence = std::make_unique<Sequence>();
    rangedSequence->addChild(std::make_unique<ConditionNode>([this](BTContext& ctx) {
        return ctx.player.getHp() > 0 &&
               ctx.distanceToPlayer <= ctx.enemy.getDetectionRange();
    }));
    rangedSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        updateAttack(ctx.dt, ctx.player);
        return NodeStatus::Running;
    }));

    auto patrolSequence = std::make_unique<Sequence>();
    patrolSequence->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.enemy.hasPatrolPoints();
    }));
    patrolSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepPatrol(ctx.dt);
    }));

    auto idleAction = std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepIdle(ctx.dt);
    });

    root->addChild(std::move(deathSequence));
    root->addChild(std::move(meleeSequence));
    root->addChild(std::move(approachSequence));
    root->addChild(std::move(rangedSequence));
    root->addChild(std::move(patrolSequence));
    root->addChild(std::move(idleAction));
    behaviorTree = std::move(root);
}

void Soldier::updateAdditionalSystems(float deltaTime, Player& player)
{
    shotTimer = std::max(0.f, shotTimer - deltaTime);

    static const Designar::DynArray<sf::FloatRect> noCollisionAreas;
    const Designar::DynArray<sf::FloatRect>& areas =
        projectileCollisionAreas != nullptr
            ? *projectileCollisionAreas
            : noCollisionAreas;

    for (std::size_t index = 0; index < bullets.size();)
    {
        if (!bullets[index].update(deltaTime, player, areas, worldBounds))
        {
            bullets.remove_pos(index);
        }
        else
        {
            ++index;
        }
    }
}

void Soldier::updateChase(float deltaTime, const Player& player)
{
    if (isFlanker)
    {
        const sf::Vector2f targetPosition =
            player.getPosition() + flankOffset;
        const sf::Vector2f difference = targetPosition - getPosition();
        const float distance =
            std::sqrt(difference.x * difference.x + difference.y * difference.y);
        setFacingRight(player.getPosition().x >= getPosition().x);

        if (distance > GameConfig::Soldier::FlankArrivalDistance)
        {
            move(difference / distance, deltaTime);
        }
        else
        {
            move({0.f, 0.f}, deltaTime);
        }
        return;
    }

    const sf::Vector2f difference = player.getPosition() - getPosition();
    setFacingRight(difference.x >= 0.f);

    if (std::abs(difference.y) > laneTolerance)
    {
        move({0.f, difference.y > 0.f ? 1.f : -1.f}, deltaTime);
        return;
    }

    if (std::abs(difference.x) > preferredDistance)
    {
        move({difference.x > 0.f ? 1.f : -1.f, 0.f}, deltaTime);
        return;
    }

    move({0.f, 0.f}, deltaTime);
}

bool Soldier::hasReachedFlankPosition(const Player& player) const
{
    const sf::Vector2f targetPosition = player.getPosition() + flankOffset;
    const sf::Vector2f difference = targetPosition - getPosition();
    return difference.x * difference.x + difference.y * difference.y <=
           GameConfig::Soldier::FlankReachedDistance *
               GameConfig::Soldier::FlankReachedDistance;
}

void Soldier::updateAttack(float deltaTime, Player& player)
{
    const sf::Vector2f difference = player.getPosition() - getPosition();
    const float horizontalDistance = std::abs(difference.x);
    const float laneDifference = difference.y;
    const bool playerIsRight = difference.x >= 0.f;
    setFacingRight(playerIsRight);

    const float distanceSquared =
        difference.x * difference.x + difference.y * difference.y;
    if (distanceSquared <= meleeRange * meleeRange)
    {
        Enemy::updateAttack(deltaTime, player);
        return;
    }

    if (std::abs(laneDifference) > laneTolerance)
    {
        move({0.f, laneDifference > 0.f ? 1.f : -1.f}, deltaTime);
        return;
    }

    if (horizontalDistance > preferredDistance)
    {
        move({playerIsRight ? 1.f : -1.f, 0.f}, deltaTime);
        return;
    }

    if (horizontalDistance < minimumDistance)
    {
        move({playerIsRight ? -1.f : 1.f, 0.f}, deltaTime);
        return;
    }

    move({0.f, 0.f}, deltaTime);
    if (shotTimer > 0.f || !hasLineOfSight(player.getPosition()))
    {
        return;
    }

    const float direction = playerIsRight ? 1.f : -1.f;
    bullets.append(Bullet(
        sf::Vector2f(
            getPosition().x + direction * muzzleOffset,
            getPosition().y - muzzleHeight),
        direction,
        projectileSpeed,
        projectileDamage));
    shotTimer = shotCooldown;
}

bool Soldier::hasLineOfSight(const sf::Vector2f& target) const
{
    if (projectileCollisionAreas == nullptr)
    {
        return true;
    }

    const float startX = getPosition().x;
    const float endX = target.x;
    const int steps = std::max(
        1,
        static_cast<int>(std::ceil(
            std::abs(endX - startX) / GameConfig::Bullet::CollisionStep)));

    for (int step = 1; step < steps; ++step)
    {
        const float fraction = static_cast<float>(step) / steps;
        const sf::Vector2f point(
            startX + (endX - startX) * fraction,
            getPosition().y - muzzleHeight);
        for (std::size_t index = 0;
             index < projectileCollisionAreas->size();
             ++index)
        {
            const sf::FloatRect& area = (*projectileCollisionAreas)[index];
            const sf::FloatRect bulletBounds(
                {point.x - GameConfig::Bullet::Radius,
                 point.y - GameConfig::Bullet::Radius},
                {GameConfig::Bullet::Radius * 2.f,
                 GameConfig::Bullet::Radius * 2.f});
            if (area.findIntersection(bulletBounds))
            {
                return false;
            }
        }
    }

    return true;
}

void Soldier::draw(sf::RenderWindow& window)
{
    Enemy::draw(window);
    for (std::size_t index = 0; index < bullets.size(); ++index)
    {
        bullets[index].draw(window);
    }
}
