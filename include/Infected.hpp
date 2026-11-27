#pragma once

#include "Enemy.hpp"
#include "GameConfig.hpp"

class Infected final : public Enemy
{
public:
    enum class Variant
    {
        Biter,
        Machete
    };

    explicit Infected(Variant enemyVariant);

    Variant getVariant() const;
    bool isFrenzied() const;
    bool consumeBleedInflicted();
    int getExperienceReward() const;

    void draw(sf::RenderWindow& window) override;

protected:
    void buildBehaviorTree() override;
    void updateAdditionalSystems(float deltaTime, Player& player) override;

private:
    enum class LungePhase
    {
        None,
        Windup,
        Dash
    };

    Variant variant;
    bool frenzied = false;
    bool bleedInflicted = false;

    LungePhase lungePhase = LungePhase::None;
    float lungePhaseTimer = 0.f;
    float lungeCooldownTimer = 0.f;
    sf::Vector2f lungeDirection{1.f, 0.f};
    bool lungeHitApplied = false;
    float visualPulseTime = 0.f;

    NodeStatus stepBiteAttack(float deltaTime, Player& player);
    NodeStatus stepMacheteLunge(float deltaTime, Player& player);
    NodeStatus stepInfectedChase(float deltaTime, const Player& player);
};