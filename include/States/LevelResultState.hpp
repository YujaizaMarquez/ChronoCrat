#pragma once

#include "State.hpp"
#include <array>
#include <string>

class LevelResultState final : public State
{
public:
    enum class Result
    {
        Victory,
        Defeat
    };

    LevelResultState(
        sf::RenderWindow* window,
        Game* game,
        State& backgroundLevel,
        Result result);

    void update(float deltaTime) override;
    void handleEvent(sf::Event& event) override;
    void draw() override;

private:
    State& backgroundLevel;
    Result result;
    std::array<sf::RectangleShape, 2> buttons;
    std::array<std::string, 2> buttonLabels;
    int selectedIndex = 0;

    void updateLayout();
    int getHoveredButton(const sf::Vector2f& mousePosition) const;
    void activateButton(int index);
    void drawPixelText(
        const std::string& text,
        const sf::Vector2f& center,
        float pixelSize,
        const sf::Color& color);
};