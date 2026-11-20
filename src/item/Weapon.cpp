#include "item/Weapon.hpp"
#include <algorithm>
#include <stdexcept>

namespace
{
constexpr int Mp40ImageWidth = 1408;
constexpr int Mp40ImageHeight = 768;
constexpr float Mp40VisualScale = 0.08f;
}

Weapon::Weapon(const std::string& imagePath)
    : Item(ItemType::Weapon, 100.f, 25.f, Mp40ImageWidth, Mp40ImageHeight)
{
    if (!loadSpriteSheet(imagePath, true))
    {
        throw std::runtime_error("No se pudo cargar la imagen del arma: " + imagePath);
    }

    setVisualScale(Mp40VisualScale);
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
    fireCooldownRemaining = FireInterval;
    return true;
}

int Weapon::getAmmo() const
{
    return ammo;
}
