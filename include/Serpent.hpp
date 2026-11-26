#pragma once

#include "Enemy.hpp"

class Serpent final : public Enemy
{
public:
    Serpent();

protected:
    void buildBehaviorTree() override;
    void updateChase(float deltaTime, const Player& player) override;
    void updateAttack(float deltaTime, Player& player) override;
};
