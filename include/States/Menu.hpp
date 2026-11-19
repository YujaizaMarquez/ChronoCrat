
#pragma once
#include <array>
#include "State.hpp"

class Menu : public State
{
private:
    // Fondo animado
    // Rectángulos de los botones
std::array<sf::RectangleShape, 3> buttons;

// Configurar los rectángulos
void initializeButtons();

// Saber qué botón está seleccionado
int getSelectedButton(sf::Vector2f mousePosition);

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Clock animationClock;
    float animationTimer = 0.f;

    static constexpr int FRAME_WIDTH = 1280;
    static constexpr int FRAME_COUNT = 4;
    static constexpr float FRAME_DURATION = 0.15f;

    int currentFrame = 0;

    // Botón X
    sf::CircleShape closeButton;
    sf::RectangleShape crossLine1;
    sf::RectangleShape crossLine2;

    sf::FloatRect closeBounds;

    bool mouseOverClose = false;

    void updateBackgroundScale();
    void updateClosePosition();

public:
    Menu(sf::RenderWindow* window, Game* game);

    void update(float deltaTime) override;
    void handleEvent(sf::Event& event) override;
    void draw() override;
};
