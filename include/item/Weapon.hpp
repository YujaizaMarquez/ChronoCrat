#pragma once
#include "item/Item.hpp"
#include "GameConfig.hpp"
#include <string>

struct WeaponLoadout
{
    std::string imagePath = GameConfig::Weapon::Mp40ImagePath;
    float damage = GameConfig::Weapon::Damage;
    int magazineCapacity = GameConfig::Weapon::MagazineCapacity;
    float fireInterval = GameConfig::Weapon::FireInterval;
};

class Weapon : public Item
{
public:
    explicit Weapon(const std::string& imagePath = "");

    static void setActiveLoadout(const WeaponLoadout& loadout);
    static const WeaponLoadout& getActiveLoadout();

    void use() override;
    bool isEquipped() const;
    void setEquipped(bool equipped);
    void updateFireCooldown(float deltaTime);
    bool tryFire();
    int getAmmo() const;

private:
    static WeaponLoadout activeLoadout;

    bool equipped = false;
    int ammo = GameConfig::Weapon::MagazineCapacity;
    float fireInterval = GameConfig::Weapon::FireInterval;
    float fireCooldownRemaining = 0.f;
};