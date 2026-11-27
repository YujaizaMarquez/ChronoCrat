#include "States/UpgradeMenuState.hpp"
#include "Game.hpp"
#include <algorithm>
#include <cctype>
#include <cstdint>

namespace
{
const std::uint8_t* getGlyph5x7(char rawChar)
{
    const char c = static_cast<char>(std::toupper(static_cast<unsigned char>(rawChar)));
    static constexpr std::uint8_t D0[7] = {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E};
    static constexpr std::uint8_t D1[7] = {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E};
    static constexpr std::uint8_t D2[7] = {0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F};
    static constexpr std::uint8_t D3[7] = {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E};
    static constexpr std::uint8_t D4[7] = {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02};
    static constexpr std::uint8_t D5[7] = {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E};
    static constexpr std::uint8_t D6[7] = {0x0E, 0x10, 0x1E, 0x11, 0x11, 0x11, 0x0E};
    static constexpr std::uint8_t D7[7] = {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08};
    static constexpr std::uint8_t D8[7] = {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E};
    static constexpr std::uint8_t D9[7] = {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E};
    static constexpr std::uint8_t A[7] = {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    static constexpr std::uint8_t B[7] = {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E};
    static constexpr std::uint8_t C[7] = {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E};
    static constexpr std::uint8_t D[7] = {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E};
    static constexpr std::uint8_t E[7] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F};
    static constexpr std::uint8_t F[7] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10};
    static constexpr std::uint8_t G[7] = {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E};
    static constexpr std::uint8_t H[7] = {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    static constexpr std::uint8_t I[7] = {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E};
    static constexpr std::uint8_t J[7] = {0x07, 0x02, 0x02, 0x02, 0x12, 0x12, 0x0C};
    static constexpr std::uint8_t K[7] = {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11};
    static constexpr std::uint8_t L[7] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F};
    static constexpr std::uint8_t M[7] = {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
    static constexpr std::uint8_t N[7] = {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11};
    static constexpr std::uint8_t O[7] = {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    static constexpr std::uint8_t P[7] = {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
    static constexpr std::uint8_t Q[7] = {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D};
    static constexpr std::uint8_t R[7] = {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11};
    static constexpr std::uint8_t S[7] = {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E};
    static constexpr std::uint8_t T[7] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
    static constexpr std::uint8_t U[7] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    static constexpr std::uint8_t V[7] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04};
    static constexpr std::uint8_t W[7] = {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11};
    static constexpr std::uint8_t X[7] = {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11};
    static constexpr std::uint8_t Y[7] = {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04};
    static constexpr std::uint8_t Z[7] = {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F};
    static constexpr std::uint8_t PLUS[7] = {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00};
    static constexpr std::uint8_t MINUS[7] = {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00};
    static constexpr std::uint8_t COLON[7] = {0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00};
    static constexpr std::uint8_t SLASH[7] = {0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10};
    static constexpr std::uint8_t DOT[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
    static constexpr std::uint8_t EXCL[7] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04};
    static constexpr std::uint8_t SPACE[7] = {0, 0, 0, 0, 0, 0, 0};

    switch (c)
    {
        case '0': return D0;
        case '1': return D1;
        case '2': return D2;
        case '3': return D3;
        case '4': return D4;
        case '5': return D5;
        case '6': return D6;
        case '7': return D7;
        case '8': return D8;
        case '9': return D9;
        case 'A': return A;
        case 'B': return B;
        case 'C': return C;
        case 'D': return D;
        case 'E': return E;
        case 'F': return F;
        case 'G': return G;
        case 'H': return H;
        case 'I': return I;
        case 'J': return J;
        case 'K': return K;
        case 'L': return L;
        case 'M': return M;
        case 'N': return N;
        case 'O': return O;
        case 'P': return P;
        case 'Q': return Q;
        case 'R': return R;
        case 'S': return S;
        case 'T': return T;
        case 'U': return U;
        case 'V': return V;
        case 'W': return W;
        case 'X': return X;
        case 'Y': return Y;
        case 'Z': return Z;
        case '+': return PLUS;
        case '-': return MINUS;
        case ':': return COLON;
        case '/': return SLASH;
        case '.': return DOT;
        case '!': return EXCL;
        default: return SPACE;
    }
}
}

UpgradeMenuState::UpgradeMenuState(
    sf::RenderWindow* window,
    Game* game,
    State& backgroundLevel,
    int earnedXp)
    : State(window, game),
      backgroundLevel(backgroundLevel),
      earnedXp(earnedXp)
{
    if (game != nullptr)
    {
        inspectedWeapon = game->getEquippedWeaponIndex();
    }
    updateLayout();
}

void UpgradeMenuState::updateLayout()
{
    const sf::Vector2u windowSize = window->getSize();
    const float winW = static_cast<float>(windowSize.x);
    const float winH = static_cast<float>(windowSize.y);

    const float panelW = std::min(960.f, winW * 0.90f);
    const float panelH = std::min(660.f, winH * 0.90f);
    const float panelLeft = (winW - panelW) * 0.5f;
    const float panelTop = (winH - panelH) * 0.5f;

    mainPanel.setSize({panelW, panelH});
    mainPanel.setPosition({panelLeft, panelTop});
    mainPanel.setFillColor(sf::Color(18, 22, 30, 240));
    mainPanel.setOutlineThickness(4.f);
    mainPanel.setOutlineColor(sf::Color(70, 210, 115));

    const float pad = 28.f;
    const float innerW = panelW - pad * 2.f;

    // Pestañas superiores (ARMAS / PERSONAJE)
    const float tabTop = panelTop + 102.f;
    const float tabGap = 16.f;
    const float tabW = (innerW - tabGap) * 0.5f;
    const float tabH = 42.f;

    weaponsTabButton.setSize({tabW, tabH});
    weaponsTabButton.setPosition({panelLeft + pad, tabTop});
    weaponsTabButton.setOutlineThickness(2.5f);

    characterTabButton.setSize({tabW, tabH});
    characterTabButton.setPosition({panelLeft + pad + tabW + tabGap, tabTop});
    characterTabButton.setOutlineThickness(2.5f);

    // Contenido pestaña Armas
    const float contentTop = panelTop + 162.f;
    const float leftColW = panelW * 0.36f;
    const float cardH = 98.f;
    const float cardGap = 14.f;

    for (std::size_t i = 0; i < weaponCards.size(); ++i)
    {
        weaponCards[i].setSize({leftColW, cardH});
        weaponCards[i].setPosition({
            panelLeft + pad,
            contentTop + static_cast<float>(i) * (cardH + cardGap)
        });
        weaponCards[i].setOutlineThickness(2.5f);
    }

    const float rightColLeft = panelLeft + pad + leftColW + 26.f;
    const float rightColW = panelLeft + panelW - pad - rightColLeft;
    const float actionBtnH = 50.f;

    equipOrBuyButton.setSize({rightColW, actionBtnH});
    equipOrBuyButton.setPosition({rightColLeft, contentTop + 150.f});
    equipOrBuyButton.setOutlineThickness(2.5f);

    upgradeDamageButton.setSize({rightColW, actionBtnH});
    upgradeDamageButton.setPosition({rightColLeft, contentTop + 212.f});
    upgradeDamageButton.setOutlineThickness(2.5f);

    upgradeAmmoButton.setSize({rightColW, actionBtnH});
    upgradeAmmoButton.setPosition({rightColLeft, contentTop + 274.f});
    upgradeAmmoButton.setOutlineThickness(2.5f);

    // Contenido pestaña Personaje
    const float statCardH = 142.f;
    const float statBtnW = 310.f;
    const float statBtnH = 58.f;

    healthCard.setSize({innerW, statCardH});
    healthCard.setPosition({panelLeft + pad, contentTop + 10.f});
    healthCard.setFillColor(sf::Color(26, 31, 42, 235));
    healthCard.setOutlineThickness(2.5f);
    healthCard.setOutlineColor(sf::Color(70, 195, 105));

    upgradeHealthButton.setSize({statBtnW, statBtnH});
    upgradeHealthButton.setPosition({
        healthCard.getPosition().x + innerW - statBtnW - 22.f,
        healthCard.getPosition().y + (statCardH - statBtnH) * 0.5f
    });
    upgradeHealthButton.setOutlineThickness(2.5f);

    speedCard.setSize({innerW, statCardH});
    speedCard.setPosition({panelLeft + pad, contentTop + 174.f});
    speedCard.setFillColor(sf::Color(26, 31, 42, 235));
    speedCard.setOutlineThickness(2.5f);
    speedCard.setOutlineColor(sf::Color(75, 190, 240));

    upgradeSpeedButton.setSize({statBtnW, statBtnH});
    upgradeSpeedButton.setPosition({
        speedCard.getPosition().x + innerW - statBtnW - 22.f,
        speedCard.getPosition().y + (statCardH - statBtnH) * 0.5f
    });
    upgradeSpeedButton.setOutlineThickness(2.5f);

    // Navegación inferior
    const float navBtnW = (innerW - 24.f) * 0.5f;
    const float navBtnH = 54.f;
    const float navTop = panelTop + panelH - pad - navBtnH;

    nextLevelButton.setSize({navBtnW, navBtnH});
    nextLevelButton.setPosition({panelLeft + pad, navTop});
    nextLevelButton.setOutlineThickness(3.f);

    mainMenuButton.setSize({navBtnW, navBtnH});
    mainMenuButton.setPosition({panelLeft + pad + navBtnW + 24.f, navTop});
    mainMenuButton.setOutlineThickness(3.f);
}

int UpgradeMenuState::getHoveredControl(const sf::Vector2f& mousePosition) const
{
    if (weaponsTabButton.getGlobalBounds().contains(mousePosition)) return 30;
    if (characterTabButton.getGlobalBounds().contains(mousePosition)) return 31;

    if (activeTab == Tab::Weapons)
    {
        for (std::size_t i = 0; i < weaponCards.size(); ++i)
        {
            if (weaponCards[i].getGlobalBounds().contains(mousePosition))
            {
                return static_cast<int>(i);
            }
        }
        if (equipOrBuyButton.getGlobalBounds().contains(mousePosition)) return 10;
        if (upgradeDamageButton.getGlobalBounds().contains(mousePosition)) return 11;
        if (upgradeAmmoButton.getGlobalBounds().contains(mousePosition)) return 12;
    }
    else
    {
        if (upgradeHealthButton.getGlobalBounds().contains(mousePosition)) return 40;
        if (upgradeSpeedButton.getGlobalBounds().contains(mousePosition)) return 41;
    }

    if (nextLevelButton.getGlobalBounds().contains(mousePosition)) return 20;
    if (mainMenuButton.getGlobalBounds().contains(mousePosition)) return 21;
    return -1;
}

void UpgradeMenuState::setFeedback(const std::string& message, const sf::Color& color)
{
    feedbackMessage = message;
    feedbackColor = color;
    feedbackTimer = 2.4f;
}

void UpgradeMenuState::handleBuyOrEquip()
{
    if (game == nullptr) return;

    const bool wasUnlocked = game->getWeaponProgress(inspectedWeapon).unlocked;
    if (wasUnlocked && game->getEquippedWeaponIndex() == inspectedWeapon)
    {
        setFeedback("YA ESTA EQUIPADA", sf::Color(180, 190, 205));
        return;
    }

    if (game->buyOrEquipWeapon(inspectedWeapon))
    {
        setFeedback(
            wasUnlocked ? "ARMA EQUIPADA!" : "ARMA COMPRADA Y EQUIPADA!",
            sf::Color(90, 235, 130));
    }
    else
    {
        setFeedback("XP INSUFICIENTE", sf::Color(240, 85, 85));
    }
}

void UpgradeMenuState::handleUpgradeDamage()
{
    if (game == nullptr) return;

    const auto& progress = game->getWeaponProgress(inspectedWeapon);
    if (!progress.unlocked)
    {
        setFeedback("COMPRA EL ARMA PRIMERO", sf::Color(240, 175, 65));
        return;
    }
    if (progress.damageLevel >= GameConfig::WeaponShop::MaxUpgradeLevel)
    {
        setFeedback("DANO AL NIVEL MAXIMO", sf::Color(245, 205, 80));
        return;
    }
    if (game->upgradeWeaponDamage(inspectedWeapon))
    {
        setFeedback("DANO MEJORADO!", sf::Color(90, 235, 130));
    }
    else
    {
        setFeedback("XP INSUFICIENTE", sf::Color(240, 85, 85));
    }
}

void UpgradeMenuState::handleUpgradeAmmo()
{
    if (game == nullptr) return;

    const auto& progress = game->getWeaponProgress(inspectedWeapon);
    if (!progress.unlocked)
    {
        setFeedback("COMPRA EL ARMA PRIMERO", sf::Color(240, 175, 65));
        return;
    }
    if (progress.ammoLevel >= GameConfig::WeaponShop::MaxUpgradeLevel)
    {
        setFeedback("CARGADOR AL NIVEL MAXIMO", sf::Color(245, 205, 80));
        return;
    }
    if (game->upgradeWeaponAmmo(inspectedWeapon))
    {
        setFeedback("CARGADOR AMPLIADO!", sf::Color(90, 235, 130));
    }
    else
    {
        setFeedback("XP INSUFICIENTE", sf::Color(240, 85, 85));
    }
}

void UpgradeMenuState::handleUpgradeHealth()
{
    if (game == nullptr) return;

    if (game->getCharacterProgress().healthLevel >=
        GameConfig::CharacterUpgrades::MaxUpgradeLevel)
    {
        setFeedback("VIDA AL NIVEL MAXIMO", sf::Color(245, 205, 80));
        return;
    }
    if (game->upgradeCharacterHealth())
    {
        setFeedback("VIDA MAXIMA MEJORADA!", sf::Color(90, 235, 130));
    }
    else
    {
        setFeedback("XP INSUFICIENTE", sf::Color(240, 85, 85));
    }
}

void UpgradeMenuState::handleUpgradeSpeed()
{
    if (game == nullptr) return;

    if (game->getCharacterProgress().speedLevel >=
        GameConfig::CharacterUpgrades::MaxUpgradeLevel)
    {
        setFeedback("VELOCIDAD AL NIVEL MAXIMO", sf::Color(245, 205, 80));
        return;
    }
    if (game->upgradeCharacterSpeed())
    {
        setFeedback("VELOCIDAD MEJORADA!", sf::Color(90, 235, 130));
    }
    else
    {
        setFeedback("XP INSUFICIENTE", sf::Color(240, 85, 85));
    }
}

void UpgradeMenuState::update(float deltaTime)
{
    feedbackTimer = std::max(0.f, feedbackTimer - deltaTime);
    const sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    hoveredControl = getHoveredControl({
        static_cast<float>(mousePos.x),
        static_cast<float>(mousePos.y)
    });
}

void UpgradeMenuState::handleEvent(sf::Event& event)
{
    if (event.is<sf::Event::Resized>())
    {
        updateLayout();
        return;
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Tab ||
            keyPressed->code == sf::Keyboard::Key::Left ||
            keyPressed->code == sf::Keyboard::Key::Right)
        {
            activeTab = activeTab == Tab::Weapons ? Tab::Character : Tab::Weapons;
            return;
        }
        if (keyPressed->code == sf::Keyboard::Key::Space)
        {
            if (game != nullptr) game->startNextLevel();
            return;
        }
        if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            if (game != nullptr) game->returnToMainMenu();
            return;
        }

        if (activeTab == Tab::Weapons)
        {
            switch (keyPressed->code)
            {
                case sf::Keyboard::Key::Num1:
                    inspectedWeapon = 0;
                    return;
                case sf::Keyboard::Key::Num2:
                    inspectedWeapon = 1;
                    return;
                case sf::Keyboard::Key::Num3:
                    inspectedWeapon = 2;
                    return;
                case sf::Keyboard::Key::Up:
                case sf::Keyboard::Key::W:
                    inspectedWeapon =
                        (inspectedWeapon + GameConfig::WeaponShop::CatalogSize - 1) %
                        GameConfig::WeaponShop::CatalogSize;
                    return;
                case sf::Keyboard::Key::Down:
                case sf::Keyboard::Key::S:
                    inspectedWeapon =
                        (inspectedWeapon + 1) % GameConfig::WeaponShop::CatalogSize;
                    return;
                case sf::Keyboard::Key::Enter:
                    handleBuyOrEquip();
                    return;
                case sf::Keyboard::Key::Q:
                    handleUpgradeDamage();
                    return;
                case sf::Keyboard::Key::E:
                    handleUpgradeAmmo();
                    return;
                default:
                    break;
            }
        }
        else
        {
            if (keyPressed->code == sf::Keyboard::Key::Q ||
                keyPressed->code == sf::Keyboard::Key::Num1)
            {
                handleUpgradeHealth();
                return;
            }
            if (keyPressed->code == sf::Keyboard::Key::E ||
                keyPressed->code == sf::Keyboard::Key::Num2)
            {
                handleUpgradeSpeed();
                return;
            }
        }
    }

    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseEvent->button != sf::Mouse::Button::Left)
        {
            return;
        }

        const int clicked = getHoveredControl({
            static_cast<float>(mouseEvent->position.x),
            static_cast<float>(mouseEvent->position.y)
        });

        if (clicked == 30)
        {
            activeTab = Tab::Weapons;
        }
        else if (clicked == 31)
        {
            activeTab = Tab::Character;
        }
        else if (clicked >= 0 &&
                 clicked < static_cast<int>(GameConfig::WeaponShop::CatalogSize))
        {
            inspectedWeapon = static_cast<std::size_t>(clicked);
        }
        else if (clicked == 10)
        {
            handleBuyOrEquip();
        }
        else if (clicked == 11)
        {
            handleUpgradeDamage();
        }
        else if (clicked == 12)
        {
            handleUpgradeAmmo();
        }
        else if (clicked == 40)
        {
            handleUpgradeHealth();
        }
        else if (clicked == 41)
        {
            handleUpgradeSpeed();
        }
        else if (clicked == 20 && game != nullptr)
        {
            game->startNextLevel();
        }
        else if (clicked == 21 && game != nullptr)
        {
            game->returnToMainMenu();
        }
    }
}

void UpgradeMenuState::draw()
{
    backgroundLevel.draw();

    window->setView(window->getDefaultView());
    const sf::Vector2u windowSize = window->getSize();

    sf::RectangleShape overlay({
        static_cast<float>(windowSize.x),
        static_cast<float>(windowSize.y)
    });
    overlay.setFillColor(sf::Color(8, 10, 16, 190));
    window->draw(overlay);

    window->draw(mainPanel);

    const sf::Vector2f panelPos = mainPanel.getPosition();
    const sf::Vector2f panelSize = mainPanel.getSize();
    const float centerX = panelPos.x + panelSize.x * 0.5f;

    drawPixelText(
        "VICTORIA - MEJORAS",
        {centerX, panelPos.y + 34.f},
        4.3f,
        sf::Color(75, 220, 120));

    const int currentXp = game != nullptr ? game->getUpgradeXp() : 0;
    const std::string xpLine =
        "XP DISPONIBLE: " + std::to_string(currentXp) +
        "   +" + std::to_string(earnedXp) + " XP";
    drawPixelText(
        xpLine,
        {centerX, panelPos.y + 74.f},
        2.6f,
        sf::Color(255, 215, 85));

    // Pestañas
    const bool weaponsActive = activeTab == Tab::Weapons;
    weaponsTabButton.setFillColor(
        weaponsActive
            ? sf::Color(46, 62, 78, 245)
            : (hoveredControl == 30 ? sf::Color(34, 42, 56, 235) : sf::Color(24, 28, 38, 230)));
    weaponsTabButton.setOutlineColor(
        weaponsActive ? sf::Color(255, 215, 85) : sf::Color(85, 95, 115));
    window->draw(weaponsTabButton);
    drawPixelText(
        "ARMAS",
        weaponsTabButton.getPosition() + weaponsTabButton.getSize() * 0.5f,
        2.7f,
        weaponsActive ? sf::Color(255, 220, 100) : sf::Color(185, 195, 210));

    characterTabButton.setFillColor(
        !weaponsActive
            ? sf::Color(46, 62, 78, 245)
            : (hoveredControl == 31 ? sf::Color(34, 42, 56, 235) : sf::Color(24, 28, 38, 230)));
    characterTabButton.setOutlineColor(
        !weaponsActive ? sf::Color(80, 225, 130) : sf::Color(85, 95, 115));
    window->draw(characterTabButton);
    drawPixelText(
        "PERSONAJE",
        characterTabButton.getPosition() + characterTabButton.getSize() * 0.5f,
        2.7f,
        !weaponsActive ? sf::Color(90, 235, 135) : sf::Color(185, 195, 210));

    if (game != nullptr)
    {
        if (weaponsActive)
        {
            drawWeaponsTab(currentXp);
        }
        else
        {
            drawCharacterTab(currentXp);
        }
    }

    if (feedbackTimer > 0.f && !feedbackMessage.empty())
    {
        drawPixelText(
            feedbackMessage,
            {centerX, nextLevelButton.getPosition().y - 32.f},
            2.3f,
            feedbackColor);
    }

    // Botones inferiores de navegación
    nextLevelButton.setFillColor(
        hoveredControl == 20
            ? sf::Color(45, 80, 56, 245)
            : sf::Color(28, 42, 36, 235));
    nextLevelButton.setOutlineColor(
        hoveredControl == 20 ? sf::Color(85, 235, 130) : sf::Color(70, 175, 105));
    window->draw(nextLevelButton);
    drawPixelText(
        "SIGUIENTE NIVEL",
        nextLevelButton.getPosition() + nextLevelButton.getSize() * 0.5f,
        2.8f,
        sf::Color::White);

    mainMenuButton.setFillColor(
        hoveredControl == 21
            ? sf::Color(60, 54, 72, 245)
            : sf::Color(32, 36, 48, 235));
    mainMenuButton.setOutlineColor(
        hoveredControl == 21 ? sf::Color::White : sf::Color(115, 125, 145));
    window->draw(mainMenuButton);
    drawPixelText(
        "MENU PRINCIPAL",
        mainMenuButton.getPosition() + mainMenuButton.getSize() * 0.5f,
        2.8f,
        sf::Color::White);
}

void UpgradeMenuState::drawWeaponsTab(int currentXp)
{
    for (std::size_t i = 0; i < weaponCards.size(); ++i)
    {
        const auto& def = GameConfig::WeaponShop::Catalog[i];
        const auto& prog = game->getWeaponProgress(i);
        const bool isInspected = i == inspectedWeapon;
        const bool isEquipped = i == game->getEquippedWeaponIndex();
        const bool isHovered = hoveredControl == static_cast<int>(i);

        weaponCards[i].setFillColor(
            isInspected
                ? sf::Color(44, 52, 70, 245)
                : (isHovered ? sf::Color(34, 40, 54, 235) : sf::Color(26, 30, 40, 230)));

        if (isEquipped)
        {
            weaponCards[i].setOutlineColor(sf::Color(255, 210, 75));
        }
        else if (isInspected)
        {
            weaponCards[i].setOutlineColor(sf::Color(80, 220, 130));
        }
        else
        {
            weaponCards[i].setOutlineColor(sf::Color(85, 95, 115));
        }

        window->draw(weaponCards[i]);

        const sf::Vector2f cardPos = weaponCards[i].getPosition();
        drawPixelTextLeft(
            def.name,
            {cardPos.x + 16.f, cardPos.y + 14.f},
            2.5f,
            sf::Color::White);

        std::string statusText;
        sf::Color statusColor;
        if (isEquipped)
        {
            statusText = "EQUIPADA";
            statusColor = sf::Color(255, 215, 75);
        }
        else if (prog.unlocked)
        {
            statusText = "DISPONIBLE";
            statusColor = sf::Color(90, 225, 135);
        }
        else
        {
            statusText = "BLOQUEADA - " + std::to_string(def.unlockCost) + " XP";
            statusColor = sf::Color(225, 135, 95);
        }

        drawPixelTextLeft(
            statusText,
            {cardPos.x + 16.f, cardPos.y + 46.f},
            2.0f,
            statusColor);

        const std::string miniStats =
            "DANO " + std::to_string(game->getWeaponDamage(i)) +
            "  BALAS " + std::to_string(game->getWeaponMagazine(i));
        drawPixelTextLeft(
            miniStats,
            {cardPos.x + 16.f, cardPos.y + 72.f},
            1.8f,
            sf::Color(170, 180, 195));
    }

    const auto& selectedDef = GameConfig::WeaponShop::Catalog[inspectedWeapon];
    const auto& selectedProg = game->getWeaponProgress(inspectedWeapon);
    const bool isEquipped = inspectedWeapon == game->getEquippedWeaponIndex();

    const sf::Vector2f rightTop = {
        equipOrBuyButton.getPosition().x,
        weaponCards[0].getPosition().y
    };
    const float rightW = equipOrBuyButton.getSize().x;

    drawPixelTextLeft(
        selectedDef.name,
        {rightTop.x, rightTop.y + 2.f},
        3.2f,
        sf::Color(255, 220, 100));

    const std::string dmgLine =
        "DANO: " + std::to_string(game->getWeaponDamage(inspectedWeapon)) +
        "   NV " + std::to_string(selectedProg.damageLevel) + "/" +
        std::to_string(GameConfig::WeaponShop::MaxUpgradeLevel);
    drawPixelTextLeft(dmgLine, {rightTop.x, rightTop.y + 38.f}, 2.1f, sf::Color::White);
    drawProgressBar(
        {rightTop.x, rightTop.y + 62.f},
        {rightW, 14.f},
        selectedProg.damageLevel,
        GameConfig::WeaponShop::MaxUpgradeLevel,
        sf::Color(235, 95, 70));

    const std::string ammoLine =
        "CARGADOR: " + std::to_string(game->getWeaponMagazine(inspectedWeapon)) +
        "   NV " + std::to_string(selectedProg.ammoLevel) + "/" +
        std::to_string(GameConfig::WeaponShop::MaxUpgradeLevel);
    drawPixelTextLeft(ammoLine, {rightTop.x, rightTop.y + 90.f}, 2.1f, sf::Color::White);
    drawProgressBar(
        {rightTop.x, rightTop.y + 114.f},
        {rightW, 14.f},
        selectedProg.ammoLevel,
        GameConfig::WeaponShop::MaxUpgradeLevel,
        sf::Color(80, 195, 245));

    std::string equipLabel;
    bool equipCanClick = true;
    if (!selectedProg.unlocked)
    {
        equipLabel = "COMPRAR ARMA - " + std::to_string(selectedDef.unlockCost) + " XP";
        equipCanClick = currentXp >= selectedDef.unlockCost;
    }
    else if (isEquipped)
    {
        equipLabel = "ARMA EQUIPADA";
        equipCanClick = false;
    }
    else
    {
        equipLabel = "EQUIPAR ARMA";
    }

    equipOrBuyButton.setFillColor(
        hoveredControl == 10 && equipCanClick
            ? sf::Color(55, 75, 58, 245)
            : sf::Color(32, 38, 50, 235));
    equipOrBuyButton.setOutlineColor(
        isEquipped
            ? sf::Color(255, 210, 75)
            : (equipCanClick ? sf::Color(85, 220, 130) : sf::Color(95, 95, 105)));
    window->draw(equipOrBuyButton);
    drawPixelText(
        equipLabel,
        equipOrBuyButton.getPosition() + equipOrBuyButton.getSize() * 0.5f,
        2.3f,
        equipCanClick || isEquipped ? sf::Color::White : sf::Color(145, 150, 160));

    const bool dmgAtMax = selectedProg.damageLevel >= GameConfig::WeaponShop::MaxUpgradeLevel;
    const int dmgCost = game->getDamageUpgradeCost(inspectedWeapon);
    const bool canUpgradeDmg = selectedProg.unlocked && !dmgAtMax && currentXp >= dmgCost;
    const std::string dmgBtnLabel = dmgAtMax
        ? "DANO AL MAXIMO"
        : "MEJORAR DANO +" + std::to_string(selectedDef.damagePerLevel) +
              " - " + std::to_string(dmgCost) + " XP";

    upgradeDamageButton.setFillColor(
        hoveredControl == 11 && canUpgradeDmg
            ? sf::Color(76, 48, 44, 245)
            : sf::Color(32, 36, 48, 235));
    upgradeDamageButton.setOutlineColor(
        canUpgradeDmg ? sf::Color(235, 105, 75) : sf::Color(85, 90, 100));
    window->draw(upgradeDamageButton);
    drawPixelText(
        dmgBtnLabel,
        upgradeDamageButton.getPosition() + upgradeDamageButton.getSize() * 0.5f,
        2.2f,
        canUpgradeDmg ? sf::Color::White : sf::Color(145, 150, 160));

    const bool ammoAtMax = selectedProg.ammoLevel >= GameConfig::WeaponShop::MaxUpgradeLevel;
    const int ammoCost = game->getAmmoUpgradeCost(inspectedWeapon);
    const bool canUpgradeAmmo = selectedProg.unlocked && !ammoAtMax && currentXp >= ammoCost;
    const std::string ammoBtnLabel = ammoAtMax
        ? "CARGADOR AL MAXIMO"
        : "MEJORAR BALAS +" + std::to_string(selectedDef.magazinePerLevel) +
              " - " + std::to_string(ammoCost) + " XP";

    upgradeAmmoButton.setFillColor(
        hoveredControl == 12 && canUpgradeAmmo
            ? sf::Color(40, 62, 78, 245)
            : sf::Color(32, 36, 48, 235));
    upgradeAmmoButton.setOutlineColor(
        canUpgradeAmmo ? sf::Color(80, 195, 245) : sf::Color(85, 90, 100));
    window->draw(upgradeAmmoButton);
    drawPixelText(
        ammoBtnLabel,
        upgradeAmmoButton.getPosition() + upgradeAmmoButton.getSize() * 0.5f,
        2.2f,
        canUpgradeAmmo ? sf::Color::White : sf::Color(145, 150, 160));
}

void UpgradeMenuState::drawCharacterTab(int currentXp)
{
    const auto& charProg = game->getCharacterProgress();
    const int maxLvl = GameConfig::CharacterUpgrades::MaxUpgradeLevel;

    // Tarjeta 1: Vida máxima
    window->draw(healthCard);
    const sf::Vector2f hpPos = healthCard.getPosition();
    const float barW = upgradeHealthButton.getPosition().x - hpPos.x - 48.f;

    drawPixelTextLeft("VIDA MAXIMA", {hpPos.x + 24.f, hpPos.y + 20.f}, 3.0f, sf::Color(90, 235, 135));
    const std::string hpStats =
        "BONO ACTUAL: +" + std::to_string(game->getBonusMaxHp()) +
        " HP   NV " + std::to_string(charProg.healthLevel) + "/" + std::to_string(maxLvl);
    drawPixelTextLeft(hpStats, {hpPos.x + 24.f, hpPos.y + 62.f}, 2.1f, sf::Color::White);
    drawProgressBar(
        {hpPos.x + 24.f, hpPos.y + 98.f},
        {barW, 18.f},
        charProg.healthLevel,
        maxLvl,
        sf::Color(70, 215, 115));

    const bool hpAtMax = charProg.healthLevel >= maxLvl;
    const int hpCost = game->getHealthUpgradeCost();
    const bool canBuyHp = !hpAtMax && currentXp >= hpCost;
    const std::string hpBtnLabel = hpAtMax
        ? "NIVEL MAXIMO"
        : "+" + std::to_string(GameConfig::CharacterUpgrades::BonusHpPerLevel) +
              " HP - " + std::to_string(hpCost) + " XP";

    upgradeHealthButton.setFillColor(
        hoveredControl == 40 && canBuyHp
            ? sf::Color(48, 86, 60, 245)
            : sf::Color(32, 38, 50, 235));
    upgradeHealthButton.setOutlineColor(
        canBuyHp ? sf::Color(85, 230, 130) : sf::Color(85, 90, 100));
    window->draw(upgradeHealthButton);
    drawPixelText(
        hpBtnLabel,
        upgradeHealthButton.getPosition() + upgradeHealthButton.getSize() * 0.5f,
        2.3f,
        canBuyHp ? sf::Color::White : sf::Color(145, 150, 160));

    // Tarjeta 2: Velocidad
    window->draw(speedCard);
    const sf::Vector2f spdPos = speedCard.getPosition();

    drawPixelTextLeft("VELOCIDAD", {spdPos.x + 24.f, spdPos.y + 20.f}, 3.0f, sf::Color(85, 205, 250));
    const std::string spdStats =
        "BONO ACTUAL: +" + std::to_string(static_cast<int>(game->getBonusSpeed())) +
        " VEL   NV " + std::to_string(charProg.speedLevel) + "/" + std::to_string(maxLvl);
    drawPixelTextLeft(spdStats, {spdPos.x + 24.f, spdPos.y + 62.f}, 2.1f, sf::Color::White);
    drawProgressBar(
        {spdPos.x + 24.f, spdPos.y + 98.f},
        {barW, 18.f},
        charProg.speedLevel,
        maxLvl,
        sf::Color(80, 195, 245));

    const bool spdAtMax = charProg.speedLevel >= maxLvl;
    const int spdCost = game->getSpeedUpgradeCost();
    const bool canBuySpd = !spdAtMax && currentXp >= spdCost;
    const std::string spdBtnLabel = spdAtMax
        ? "NIVEL MAXIMO"
        : "+" + std::to_string(static_cast<int>(GameConfig::CharacterUpgrades::BonusSpeedPerLevel)) +
              " VEL - " + std::to_string(spdCost) + " XP";

    upgradeSpeedButton.setFillColor(
        hoveredControl == 41 && canBuySpd
            ? sf::Color(40, 68, 86, 245)
            : sf::Color(32, 38, 50, 235));
    upgradeSpeedButton.setOutlineColor(
        canBuySpd ? sf::Color(80, 200, 250) : sf::Color(85, 90, 100));
    window->draw(upgradeSpeedButton);
    drawPixelText(
        spdBtnLabel,
        upgradeSpeedButton.getPosition() + upgradeSpeedButton.getSize() * 0.5f,
        2.3f,
        canBuySpd ? sf::Color::White : sf::Color(145, 150, 160));
}

void UpgradeMenuState::drawProgressBar(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    int level,
    int maxLevel,
    const sf::Color& fillColor)
{
    const float gap = 6.f;
    const float segW = (size.x - gap * static_cast<float>(maxLevel - 1)) /
                       static_cast<float>(maxLevel);

    for (int i = 0; i < maxLevel; ++i)
    {
        sf::RectangleShape segment({segW, size.y});
        segment.setPosition({position.x + static_cast<float>(i) * (segW + gap), position.y});
        segment.setFillColor(i < level ? fillColor : sf::Color(42, 46, 58));
        window->draw(segment);
    }
}

void UpgradeMenuState::drawPixelText(
    const std::string& text,
    const sf::Vector2f& center,
    float pixelSize,
    const sf::Color& color)
{
    if (text.empty()) return;

    const float charAdvance = 6.f * pixelSize;
    const float totalWidth =
        static_cast<float>(text.size()) * charAdvance - pixelSize;
    const float totalHeight = 7.f * pixelSize;

    drawPixelTextLeft(
        text,
        {center.x - totalWidth * 0.5f, center.y - totalHeight * 0.5f},
        pixelSize,
        color);
}

void UpgradeMenuState::drawPixelTextLeft(
    const std::string& text,
    const sf::Vector2f& topLeft,
    float pixelSize,
    const sf::Color& color)
{
    if (text.empty()) return;

    const float charAdvance = 6.f * pixelSize;
    sf::RectangleShape pixel({pixelSize, pixelSize});
    pixel.setFillColor(color);

    for (std::size_t i = 0; i < text.size(); ++i)
    {
        const std::uint8_t* glyph = getGlyph5x7(text[i]);
        const float charX = topLeft.x + static_cast<float>(i) * charAdvance;

        for (int row = 0; row < 7; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                if ((glyph[row] & (1 << (4 - col))) != 0)
                {
                    pixel.setPosition({
                        charX + static_cast<float>(col) * pixelSize,
                        topLeft.y + static_cast<float>(row) * pixelSize
                    });
                    window->draw(pixel);
                }
            }
        }
    }
}