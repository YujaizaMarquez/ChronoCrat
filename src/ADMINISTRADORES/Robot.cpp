#include "Robot.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

Robot::Robot()
    : Enemy(GameConfig::Robot::DataPath)
{
    setCombatStats(
        GameConfig::Robot::DetectionRange,
        meleeRange,
        GameConfig::Robot::MeleeDamage,
        GameConfig::Robot::MeleeCooldown,
        GameConfig::Robot::ExperienceReward);
}

void Robot::setCollisionGeometry(
    const Designar::DynArray<sf::FloatRect>& projectileAreas,
    const sf::FloatRect& bounds)
{
    projectileCollisionAreas = &projectileAreas;
    worldBounds = bounds;
}

void Robot::buildBehaviorTree()
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

    // 2. Ataque cuerpo a cuerpo si está pegado al jugador
    auto meleeSequence = std::make_unique<Sequence>();
    meleeSequence->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.player.getHp() > 0 &&
               ctx.distanceToPlayer <= ctx.enemy.getAttackRange();
    }));
    meleeSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        setFacingRight(ctx.player.getPosition().x >= getPosition().x);
        return stepAttack(ctx.dt, ctx.player);
    }));

    // 3. Si el jugador entra en distancia corta, acorta para golpear cuerpo a cuerpo
    auto closeMeleeChaseSequence = std::make_unique<Sequence>();
    closeMeleeChaseSequence->addChild(std::make_unique<ConditionNode>([this](BTContext& ctx) {
        return ctx.player.getHp() > 0 &&
               ctx.distanceToPlayer <= meleeChaseRange;
    }));
    closeMeleeChaseSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepChase(ctx.dt, ctx.player);
    }));

    // 4. Disparo láser si está en rango, alineado en Y y con línea de visión
    auto laserSequence = std::make_unique<Sequence>();
    laserSequence->addChild(std::make_unique<ConditionNode>([this](BTContext& ctx) {
        return ctx.player.getHp() > 0 &&
               ctx.distanceToPlayer <= laserRange &&
               isAlignedForLaser(ctx.player) &&
               hasLineOfSight(ctx.player.getPosition());
    }));
    laserSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepLaserAttack(ctx.dt, ctx.player);
    }));

    // 5. Reposicionarse para alinear el disparo láser
    auto positionSequence = std::make_unique<Sequence>();
    positionSequence->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.player.getHp() > 0 &&
               ctx.distanceToPlayer <= ctx.enemy.getDetectionRange();
    }));
    positionSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepPositionForLaser(ctx.dt, ctx.player);
    }));

    // 6. Patrullaje
    auto patrolSequence = std::make_unique<Sequence>();
    patrolSequence->addChild(std::make_unique<ConditionNode>([](BTContext& ctx) {
        return ctx.enemy.hasPatrolPoints();
    }));
    patrolSequence->addChild(std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepPatrol(ctx.dt);
    }));

    // 7. Reposo por defecto
    auto idleAction = std::make_unique<ActionNode>([this](BTContext& ctx) {
        return stepIdle(ctx.dt);
    });

    root->addChild(std::move(deathSequence));
    root->addChild(std::move(meleeSequence));
    root->addChild(std::move(closeMeleeChaseSequence));
    root->addChild(std::move(laserSequence));
    root->addChild(std::move(positionSequence));
    root->addChild(std::move(patrolSequence));
    root->addChild(std::move(idleAction));

    behaviorTree = std::move(root);
}

void Robot::updateAdditionalSystems(float deltaTime, Player& player)
{
    laserTimer = std::max(0.f, laserTimer - deltaTime);

    static const Designar::DynArray<sf::FloatRect> noCollisionAreas;
    const Designar::DynArray<sf::FloatRect>& areas =
        projectileCollisionAreas != nullptr
            ? *projectileCollisionAreas
            : noCollisionAreas;

    for (std::size_t index = 0; index < lasers.size();)
    {
        if (!lasers[index].update(deltaTime, player, areas, worldBounds))
        {
            lasers.remove_pos(index);
        }
        else
        {
            ++index;
        }
    }
}

bool Robot::isAlignedForLaser(const Player& player) const
{
    const float laneDifference = std::abs(player.getPosition().y - getPosition().y);
    return laneDifference <= laneTolerance;
}

bool Robot::hasLineOfSight(const sf::Vector2f& target) const
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
        const float fraction = static_cast<float>(step) / static_cast<float>(steps);
        const sf::Vector2f point(
            startX + (endX - startX) * fraction,
            getPosition().y - muzzleHeight);
        const sf::FloatRect laserBounds(
            {point.x - GameConfig::Bullet::Radius,
             point.y - GameConfig::Bullet::Radius},
            {GameConfig::Bullet::Radius * 2.f,
             GameConfig::Bullet::Radius * 2.f});

        for (std::size_t index = 0; index < projectileCollisionAreas->size(); ++index)
        {
            if ((*projectileCollisionAreas)[index].findIntersection(laserBounds))
            {
                return false;
            }
        }
    }

    return true;
}

NodeStatus Robot::stepLaserAttack(float deltaTime, const Player& player)
{
    const bool playerIsRight = player.getPosition().x >= getPosition().x;
    setFacingRight(playerIsRight);
    move({0.f, 0.f}, deltaTime);

    if (laserTimer > 0.f)
    {
        return NodeStatus::Running;
    }

    const float direction = playerIsRight ? 1.f : -1.f;
    lasers.append(Bullet(
        sf::Vector2f(
            getPosition().x + direction * muzzleOffset,
            getPosition().y - muzzleHeight),
        direction,
        laserSpeed,
        laserDamage,
        GameConfig::Robot::LaserColor));
    laserTimer = laserCooldown;
    return NodeStatus::Success;
}

NodeStatus Robot::stepPositionForLaser(float deltaTime, const Player& player)
{
    const sf::Vector2f difference = player.getPosition() - getPosition();
    setFacingRight(difference.x >= 0.f);

    if (std::abs(difference.y) > laneTolerance)
    {
        move({0.f, difference.y > 0.f ? 1.f : -1.f}, deltaTime);
        return NodeStatus::Running;
    }

    if (std::abs(difference.x) > laserRange || !hasLineOfSight(player.getPosition()))
    {
        move({difference.x > 0.f ? 1.f : -1.f, 0.f}, deltaTime);
        return NodeStatus::Running;
    }

    move({0.f, 0.f}, deltaTime);
    return NodeStatus::Running;
}

void Robot::draw(sf::RenderWindow& window)
{
    Enemy::draw(window);
    for (std::size_t index = 0; index < lasers.size(); ++index)
    {
        lasers[index].draw(window);
    }
}