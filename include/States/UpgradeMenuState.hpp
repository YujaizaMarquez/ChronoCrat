#pragma once

#include "GameConfig.hpp"
#include "State.hpp"
#include <array>
#include <cstddef>
#include <string>

class UpgradeMenuState final : public State
{
public:
    UpgradeMenuState(
        sf::RenderWindow* window,
        Game* game,
        State& backgroundLevel,
        int earnedXp);

    void update(float deltaTime) override;
    void handleEvent(sf::Event& event) override;
    void draw() override;

private:
    enum class Tab
    {
        Weapons,
        Character
    };

    State& backgroundLevel;
    int earnedXp = 0;
    Tab activeTab = Tab::Weapons;
    std::size_t inspectedWeapon = 0;

    sf::RectangleShape mainPanel;
    sf::RectangleShape weaponsTabButton;
    sf::RectangleShape characterTabButton;

    // Pestaña Armas
    std::array<sf::RectangleShape, GameConfig::WeaponShop::CatalogSize> weaponCards;
    sf::RectangleShape equipOrBuyButton;
    sf::RectangleShape upgradeDamageButton;
    sf::RectangleShape upgradeAmmoButton;

    // Pestaña Personaje
    sf::RectangleShape healthCard;
    sf::RectangleShape speedCard;
    sf::RectangleShape upgradeHealthButton;
    sf::RectangleShape upgradeSpeedButton;

    // Navegación inferior
    sf::RectangleShape nextLevelButton;
    sf::RectangleShape mainMenuButton;

    int hoveredControl = -1;
    std::string feedbackMessage;
    sf::Color feedbackColor{230, 230, 230};
    float feedbackTimer = 0.f;

    void updateLayout();
    int getHoveredControl(const sf::Vector2f& mousePosition) const;
    void setFeedback(const std::string& message, const sf::Color& color);
    void handleBuyOrEquip();
    void handleUpgradeDamage();
    void handleUpgradeAmmo();
    void handleUpgradeHealth();
    void handleUpgradeSpeed();
    void drawWeaponsTab(int currentXp);
    void drawCharacterTab(int currentXp);
    void drawProgressBar(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        int level,
        int maxLevel,
        const sf::Color& fillColor);
    void drawPixelText(
        const std::string& text,
        const sf::Vector2f& center,
        float pixelSize,
        const sf::Color& color);
    void drawPixelTextLeft(
        const std::string& text,
        const sf::Vector2f& topLeft,
        float pixelSize,
        const sf::Color& color);
};