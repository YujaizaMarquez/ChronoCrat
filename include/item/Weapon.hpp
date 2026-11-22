#pragma once
#include "item/Item.hpp"
#include "GameConfig.hpp"
#include <string>

class Weapon : public Item
{
public:
    explicit Weapon(
        const std::string& imagePath = GameConfig::Weapon::Mp40ImagePath);

    void use() override;
    bool isEquipped() const;
    void updateFireCooldown(float deltaTime);
    bool tryFire();
    int getAmmo() const;

private:
    bool equipped = false;
    int ammo = GameConfig::Weapon::MagazineCapacity;
    float fireCooldownRemaining = 0.f;
};
