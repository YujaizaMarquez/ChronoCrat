#pragma once

#include "Enemy.hpp"

class Scientist : public Enemy
{
public:
    Scientist();

protected:
    void updateChase(
        float deltaTime,
        const Player& player) override;
};