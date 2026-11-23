#pragma once

#include "State.hpp"
#include <array>

class LevelState;

class OptionsState final : public State
{
public:
    OptionsState(sf::RenderWindow* window, Game* game, LevelState& pausedLevel);

    void update(float deltaTime) override;
    void handleEvent(sf::Event& event) override;
    void draw() override;

private:
    LevelState& pausedLevel;
    sf::Font font;
    sf::RectangleShape backdrop;
    sf::RectangleShape panel;
    std::array<sf::RectangleShape, 2> buttons;
    sf::Text title;
    sf::Text continueLabel;
    sf::Text mainMenuLabel;
    int hoveredButton = -1;

    void updateLayout();
    void updateHover();
    int getSelectedButton(sf::Vector2f mousePosition) const;
};
