#include "States/Menu.hpp"
#include <stdexcept>

Menu::Menu(sf::RenderWindow* window, Game* game) : State(window, game), backgroundSprite(backgroundTexture)
{
    if (!backgroundTexture.loadFromFile("assets/fondo.png")) {
        throw std::runtime_error("No se pudo cargar assets/fondo.png");
    }

    const sf::Vector2u textureSize = backgroundTexture.getSize();
    backgroundSprite.setTextureRect(sf::IntRect(
        {0, 0},
        {static_cast<int>(textureSize.x), static_cast<int>(textureSize.y)}
    ));

    const sf::Vector2u windowSize = window->getSize();
    backgroundSprite.setScale({
        static_cast<float>(windowSize.x) / textureSize.x,
        static_cast<float>(windowSize.y) / textureSize.y
    });
}

void Menu::update(float) {}

void Menu::handleEvent(sf::Event& event)
{
    if (const auto* resized = event.getIf<sf::Event::Resized>()) {
        const sf::Vector2u textureSize = backgroundTexture.getSize();
        backgroundSprite.setScale({
            static_cast<float>(resized->size.x) / textureSize.x,
            static_cast<float>(resized->size.y) / textureSize.y
        });
    }
}

void Menu::draw()
{
    window->draw(backgroundSprite);
}
