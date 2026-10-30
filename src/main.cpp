#include <SFML/Graphics.hpp>
#include <iostream>

int main() {

    auto window = sf::RenderWindow(sf::VideoMode({1024, 720}), "ChronoCrat - Fondo con SFML 3");
    window.setFramerateLimit(60);

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/fondo.jpeg")) {
        std::cerr << "Error al cargar la imagen de fondo en assets/fondo.jpeg" << std::endl;
        return -1;
    }
    backgroundTexture.setSmooth(false);

    sf::Sprite backgroundSprite(backgroundTexture);

    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = backgroundTexture.getSize();
    backgroundSprite.setScale({
        static_cast<float>(windowSize.x) / textureSize.x,
        static_cast<float>(windowSize.y) / textureSize.y
    });

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();

        window.draw(backgroundSprite);
        window.display();
    }

    return 0;
}