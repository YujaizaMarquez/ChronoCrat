#pragma once

#include "GameConfig.hpp"
#include "item/Item.hpp"
#include <string>

class Medkit final : public Item
{
public:
    explicit Medkit(
        const std::string& imagePath = GameConfig::Medkit::ImagePath,
        int healAmount = GameConfig::Medkit::HealAmount);

    void use() override;
    int getHealAmount() const;
    bool isConsumed() const;

private:
    int healAmount;
    bool consumed = false;
};