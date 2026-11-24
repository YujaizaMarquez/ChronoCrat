#include "States/LevelResultState.hpp"
#include "Game.hpp"
#include <cctype>
#include <cstdint>

namespace
{
// Fuente bitmap 5x7 para caracteres usados en los menús (sin depender de .ttf)
const std::uint8_t* getGlyph5x7(char rawChar)
{
    const char c = static_cast<char>(std::toupper(static_cast<unsigned char>(rawChar)));
    static constexpr std::uint8_t A[7] = {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    static constexpr std::uint8_t B[7] = {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E};
    static constexpr std::uint8_t C[7] = {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E};
    static constexpr std::uint8_t D[7] = {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E};
    static constexpr std::uint8_t E[7] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F};
    static constexpr std::uint8_t F[7] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10};
    static constexpr std::uint8_t G[7] = {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E};
    static constexpr std::uint8_t H[7] = {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    static constexpr std::uint8_t I[7] = {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E};
    static constexpr std::uint8_t L[7] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F};
    static constexpr std::uint8_t M[7] = {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
    static constexpr std::uint8_t N[7] = {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11};
    static constexpr std::uint8_t O[7] = {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    static constexpr std::uint8_t P[7] = {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
    static constexpr std::uint8_t R[7] = {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11};
    static constexpr std::uint8_t S[7] = {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E};
    static constexpr std::uint8_t T[7] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
    static constexpr std::uint8_t U[7] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    static constexpr std::uint8_t V[7] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04};
    static constexpr std::uint8_t SPACE[7] = {0, 0, 0, 0, 0, 0, 0};

    switch (c)
    {
        case 'A': return A;
        case 'B': return B;
        case 'C': return C;
        case 'D': return D;
        case 'E': return E;
        case 'F': return F;
        case 'G': return G;
        case 'H': return H;
        case 'I': return I;
        case 'L': return L;
        case 'M': return M;
        case 'N': return N;
        case 'O': return O;
        case 'P': return P;
        case 'R': return R;
        case 'S': return S;
        case 'T': return T;
        case 'U': return U;
        case 'V': return V;
        default: return SPACE;
    }
}
}

LevelResultState::LevelResultState(
    sf::RenderWindow* window,
    Game* game,
    State& backgroundLevel,
    Result result)
    : State(window, game),
      backgroundLevel(backgroundLevel),
      result(result)
{
    if (result == Result::Victory)
    {
        buttonLabels[0] = "SIGUIENTE NIVEL";
    }
    else
    {
        buttonLabels[0] = "REINTENTAR";
    }
    buttonLabels[1] = "MENU PRINCIPAL";

    updateLayout();
}

void LevelResultState::updateLayout()
{
    const sf::Vector2u windowSize = window->getSize();
    const float centerX = static_cast<float>(windowSize.x) * 0.5f;
    const float centerY = static_cast<float>(windowSize.y) * 0.5f;

    const sf::Vector2f buttonSize(360.f, 68.f);
    const float firstButtonY = centerY + 10.f;
    const float spacing = 92.f;

    for (std::size_t i = 0; i < buttons.size(); ++i)
    {
        buttons[i].setSize(buttonSize);
        buttons[i].setOrigin({buttonSize.x * 0.5f, buttonSize.y * 0.5f});
        buttons[i].setPosition({
            centerX,
            firstButtonY + static_cast<float>(i) * spacing
        });
        buttons[i].setOutlineThickness(3.f);
    }
}

int LevelResultState::getHoveredButton(const sf::Vector2f& mousePosition) const
{
    for (std::size_t i = 0; i < buttons.size(); ++i)
    {
        if (buttons[i].getGlobalBounds().contains(mousePosition))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void LevelResultState::activateButton(int index)
{
    if (game == nullptr)
    {
        return;
    }

    if (index == 0)
    {
        if (result == Result::Victory)
        {
            game->startNextLevel();
        }
        else
        {
            game->retryCurrentLevel();
        }
        return;
    }

    if (index == 1)
    {
        game->returnToMainMenu();
    }
}

void LevelResultState::update(float)
{
    const sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    const int hovered = getHoveredButton({
        static_cast<float>(mousePos.x),
        static_cast<float>(mousePos.y)
    });
    if (hovered >= 0)
    {
        selectedIndex = hovered;
    }
}

void LevelResultState::handleEvent(sf::Event& event)
{
    if (event.is<sf::Event::Resized>())
    {
        updateLayout();
        return;
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Up ||
            keyPressed->code == sf::Keyboard::Key::W)
        {
            selectedIndex = (selectedIndex + 1) % 2;
            return;
        }
        if (keyPressed->code == sf::Keyboard::Key::Down ||
            keyPressed->code == sf::Keyboard::Key::S)
        {
            selectedIndex = (selectedIndex + 1) % 2;
            return;
        }
        if (keyPressed->code == sf::Keyboard::Key::Enter ||
            keyPressed->code == sf::Keyboard::Key::Space)
        {
            activateButton(selectedIndex);
            return;
        }
        if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            activateButton(1);
            return;
        }
    }

    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseEvent->button == sf::Mouse::Button::Left)
        {
            const int clicked = getHoveredButton({
                static_cast<float>(mouseEvent->position.x),
                static_cast<float>(mouseEvent->position.y)
            });
            if (clicked >= 0)
            {
                activateButton(clicked);
            }
        }
    }
}

void LevelResultState::draw()
{
    // 1. Dibujar el nivel congelado de fondo
    backgroundLevel.draw();

    // 2. Capa oscura semitransparente
    window->setView(window->getDefaultView());
    const sf::Vector2u windowSize = window->getSize();
    const float centerX = static_cast<float>(windowSize.x) * 0.5f;
    const float centerY = static_cast<float>(windowSize.y) * 0.5f;

    sf::RectangleShape overlay({
        static_cast<float>(windowSize.x),
        static_cast<float>(windowSize.y)
    });
    overlay.setFillColor(sf::Color(10, 12, 18, 175));
    window->draw(overlay);

    // 3. Panel central
    const sf::Color accentColor = result == Result::Victory
        ? sf::Color(70, 210, 115)
        : sf::Color(220, 65, 65);

    sf::RectangleShape panel({520.f, 360.f});
    panel.setOrigin({260.f, 180.f});
    panel.setPosition({centerX, centerY + 10.f});
    panel.setFillColor(sf::Color(22, 26, 36, 235));
    panel.setOutlineColor(accentColor);
    panel.setOutlineThickness(4.f);
    window->draw(panel);

    // 4. Título (VICTORIA / DERROTA)
    const std::string title = result == Result::Victory ? "VICTORIA" : "DERROTA";
    drawPixelText(title, {centerX, centerY - 105.f}, 8.f, accentColor);

    // 5. Botones
    for (std::size_t i = 0; i < buttons.size(); ++i)
    {
        const bool isSelected = static_cast<int>(i) == selectedIndex;
        buttons[i].setFillColor(
            isSelected
                ? sf::Color(48, 56, 74, 245)
                : sf::Color(32, 36, 48, 230));
        buttons[i].setOutlineColor(
            isSelected
                ? accentColor
                : sf::Color(110, 118, 135));
        window->draw(buttons[i]);

        drawPixelText(
            buttonLabels[i],
            buttons[i].getPosition(),
            3.5f,
            isSelected ? sf::Color::White : sf::Color(200, 205, 215));
    }
}

void LevelResultState::drawPixelText(
    const std::string& text,
    const sf::Vector2f& center,
    float pixelSize,
    const sf::Color& color)
{
    if (text.empty())
    {
        return;
    }

    const float charAdvance = 6.f * pixelSize;
    const float totalWidth =
        static_cast<float>(text.size()) * charAdvance - pixelSize;
    const float totalHeight = 7.f * pixelSize;
    const float startX = center.x - totalWidth * 0.5f;
    const float startY = center.y - totalHeight * 0.5f;

    sf::RectangleShape pixel({pixelSize, pixelSize});
    pixel.setFillColor(color);

    for (std::size_t i = 0; i < text.size(); ++i)
    {
        const std::uint8_t* glyph = getGlyph5x7(text[i]);
        const float charX = startX + static_cast<float>(i) * charAdvance;

        for (int row = 0; row < 7; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                if ((glyph[row] & (1 << (4 - col))) != 0)
                {
                    pixel.setPosition({
                        charX + static_cast<float>(col) * pixelSize,
                        startY + static_cast<float>(row) * pixelSize
                    });
                    window->draw(pixel);
                }
            }
        }
    }
}