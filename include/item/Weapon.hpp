#pragma once

#include "item/Item.hpp"
#include <string>

class Weapon : public Item
{
public:
    static constexpr int MagazineCapacity = 30;

    explicit Weapon(
        const std::string& imagePath = "assets/era1/guerra/mp40.jpeg");

    void use() override;
    bool isEquipped() const;
    void updateFireCooldown(float deltaTime);
    bool tryFire();
    int getAmmo() const;

private:
    bool equipped = false;
    int ammo = MagazineCapacity;
    float fireCooldownRemaining = 0.f;
    static constexpr float FireInterval = 0.18f;
};
