
#pragma once

#include "State.hpp"
#include <array>
#include <SFML/Graphics.hpp>

class LevelSelectionMenu : public State
{
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    float animationTimer = 0.f;
    float pulseTimer = 0.f;
    int currentFrame = 0;

    // Raíz: jefe final.
    sf::CircleShape root;

    // Hijos: jefes de las tres eras.
    std::array<sf::CircleShape, 3> eras;

    // Nietos: niveles 1 y 2 de cada era.
    std::array<sf::CircleShape, 3> selectableLevels;
    std::array<sf::CircleShape, 3> lockedLevels;

    std::array<sf::Vector2f, 3> eraPositions;
    std::array<sf::Vector2f, 3> selectablePositions;
    std::array<sf::Vector2f, 3> lockedPositions;

    int hoveredLevel = -1;

    void updateBackgroundScale();
    void updateNodePulse();
    void initializeTree();
    void updateHover();
    void updateNodeColors();

    // Devuelve el identificador del círculo seleccionado.
    int getSelectedNode(sf::Vector2f mousePosition) const;

    void drawConnection(
        sf::Vector2f start,
        sf::Vector2f end
    );

public:
    LevelSelectionMenu(sf::RenderWindow* window, Game* game);

    void update(float deltaTime) override;
    void handleEvent(sf::Event& event) override;
    void draw() override;
};
