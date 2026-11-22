#include "item/Weapon.hpp"
#include <algorithm>
#include <stdexcept>

Weapon::Weapon(const std::string& imagePath)
    : Item(
          ItemType::Weapon,
          GameConfig::Weapon::Durability,
          GameConfig::Weapon::Damage,
          GameConfig::Weapon::Mp40ImageWidth,
          GameConfig::Weapon::Mp40ImageHeight)
{
    if (!loadSpriteSheet(imagePath, true))
    {
        throw std::runtime_error("No se pudo cargar la imagen del arma: " + imagePath);
    }

    setVisualScale(GameConfig::Weapon::Mp40VisualScale);
}

void Weapon::use()
{
    equipped = true;
}

bool Weapon::isEquipped() const
{
    return equipped;
}

void Weapon::updateFireCooldown(float deltaTime)
{
    fireCooldownRemaining =
        std::max(0.f, fireCooldownRemaining - std::max(0.f, deltaTime));
}

bool Weapon::tryFire()
{
    if (!equipped || ammo <= 0 || fireCooldownRemaining > 0.f)
    {
        return false;
    }

    --ammo;
    fireCooldownRemaining = GameConfig::Weapon::FireInterval;
    return true;
}

int Weapon::getAmmo() const
{
    return ammo;
}
