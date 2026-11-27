#include "item/Weapon.hpp"
#include <algorithm>
#include <stdexcept>

WeaponLoadout Weapon::activeLoadout{};

Weapon::Weapon(const std::string& imagePath)
    : Item(
          ItemType::Weapon,
          GameConfig::Weapon::Durability,
          activeLoadout.damage,
          GameConfig::Weapon::Mp40ImageWidth,
          GameConfig::Weapon::Mp40ImageHeight),
      ammo(activeLoadout.magazineCapacity),
      fireInterval(activeLoadout.fireInterval)
{
    const std::string resolvedPath =
        imagePath.empty() ? activeLoadout.imagePath : imagePath;

    if (!loadSpriteSheet(resolvedPath, true))
    {
        throw std::runtime_error("No se pudo cargar la imagen del arma: " + resolvedPath);
    }

    setVisualScale(GameConfig::Weapon::Mp40VisualScale);
}

void Weapon::setActiveLoadout(const WeaponLoadout& loadout)
{
    activeLoadout = loadout;
}

const WeaponLoadout& Weapon::getActiveLoadout()
{
    return activeLoadout;
}

void Weapon::use()
{
    equipped = true;
}

bool Weapon::isEquipped() const
{
    return equipped;
}

void Weapon::setEquipped(bool isEquipped)
{
    equipped = isEquipped;
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
    fireCooldownRemaining = fireInterval;
    return true;
}

int Weapon::getAmmo() const
{
    return ammo;
}