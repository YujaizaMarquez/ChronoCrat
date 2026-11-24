#include "States/OptionsState.hpp"
#include "Game.hpp"
#include "levels/LevelState.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>

namespace
{

std::filesystem::path getSystemFontPath()
{
    const char* windowsDirectory = std::getenv("WINDIR");
    if (windowsDirectory == nullptr)
    {
        throw std::runtime_error(
            "No se encontró la variable de entorno WINDIR para cargar la fuente");
    }

    return std::filesystem::path(windowsDirectory) / "Fonts" / "arial.ttf";
}

void centerText(sf::Text& text, const sf::Vector2f& position)
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    });
    text.setPosition(position);
}

}

OptionsState::OptionsState(
    sf::RenderWindow* window,
    Game* game,
    LevelState& pausedLevel)
    : State(window, game),
      pausedLevel(pausedLevel),
      font(getSystemFontPath()),
      title(font, "PAUSA"),
      continueLabel(font, "Continuar"),
      mainMenuLabel(font, "Volver al menú principal")
{
    title.setStyle(sf::Text::Bold);
    continueLabel.setStyle(sf::Text::Bold);
    mainMenuLabel.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color::White);
    continueLabel.setFillColor(sf::Color::White);
    mainMenuLabel.setFillColor(sf::Color::White);
    updateLayout();
}

void OptionsState::update(float)
{
    updateHover();
}

void OptionsState::handleEvent(sf::Event& event)
{
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            game->popState();
            return;
        }
    }

    if (event.is<sf::Event::Resized>())
    {
        updateLayout();
        return;
    }

    const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
    if (mouseEvent == nullptr ||
        mouseEvent->button != sf::Mouse::Button::Left)
    {
        return;
    }

    const int selectedButton = getSelectedButton({
        static_cast<float>(mouseEvent->position.x),
        static_cast<float>(mouseEvent->position.y)
    });

    if (selectedButton == 0)
    {
        game->popState();
    }
    else if (selectedButton == 1)
    {
        game->returnToMainMenu();
    }
}

void OptionsState::draw()
{
    pausedLevel.draw();
    window->draw(backdrop);
    window->draw(panel);
    window->draw(title);

    for (const sf::RectangleShape& button : buttons)
    {
        window->draw(button);
    }

    window->draw(continueLabel);
    window->draw(mainMenuLabel);
}

void OptionsState::updateLayout()
{
    const sf::Vector2u windowSize = window->getSize();
    const float width = static_cast<float>(windowSize.x);
    const float height = static_cast<float>(windowSize.y);
    const float scale = std::max(
        0.5f,
        std::min(width / 1920.f, height / 1080.f));
    const sf::Vector2f center(width / 2.f, height / 2.f);
    const sf::Vector2f panelSize(760.f * scale, 480.f * scale);
    const sf::Vector2f buttonSize(620.f * scale, 86.f * scale);

    backdrop.setSize({width, height});
    backdrop.setFillColor(sf::Color(0, 0, 0, 190));

    panel.setSize(panelSize);
    panel.setOrigin(panelSize / 2.f);
    panel.setPosition(center);
    panel.setFillColor(sf::Color(24, 30, 40, 250));
    panel.setOutlineColor(sf::Color(180, 195, 210));
    panel.setOutlineThickness(3.f * scale);

    for (std::size_t index = 0; index < buttons.size(); ++index)
    {
        buttons[index].setSize(buttonSize);
        buttons[index].setOrigin(buttonSize / 2.f);
        buttons[index].setPosition({
            center.x,
            center.y + (index == 0 ? -25.f : 85.f) * scale
        });
        buttons[index].setFillColor(sf::Color(48, 62, 78));
        buttons[index].setOutlineColor(sf::Color(150, 170, 190));
        buttons[index].setOutlineThickness(2.f * scale);
    }

    title.setCharacterSize(static_cast<unsigned int>(52.f * scale));
    continueLabel.setCharacterSize(static_cast<unsigned int>(34.f * scale));
    mainMenuLabel.setCharacterSize(static_cast<unsigned int>(34.f * scale));
    centerText(title, {center.x, center.y - 145.f * scale});
    centerText(continueLabel, buttons[0].getPosition());
    centerText(mainMenuLabel, buttons[1].getPosition());
}

void OptionsState::updateHover()
{
    const sf::Vector2i mousePosition = sf::Mouse::getPosition(*window);
    hoveredButton = getSelectedButton({
        static_cast<float>(mousePosition.x),
        static_cast<float>(mousePosition.y)
    });

    for (std::size_t index = 0; index < buttons.size(); ++index)
    {
        buttons[index].setFillColor(
            static_cast<int>(index) == hoveredButton
                ? sf::Color(72, 112, 150)
                : sf::Color(48, 62, 78));
    }
}

int OptionsState::getSelectedButton(sf::Vector2f mousePosition) const
{
    for (std::size_t index = 0; index < buttons.size(); ++index)
    {
        if (buttons[index].getGlobalBounds().contains(mousePosition))
        {
            return static_cast<int>(index);
        }
    }

    return -1;
}
