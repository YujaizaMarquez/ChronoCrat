#pragma once
#include <SFML/Graphics.hpp>

class Game;

class State
{
    protected:
        sf::RenderWindow* window;
        Game* game;
    public:
        State(sf::RenderWindow* window, Game* game) : window(window), game(game) {}
        virtual void update(float deltaTime) = 0;
        virtual void handleEvent(sf::Event& event) = 0;
        virtual void draw() = 0;
        virtual ~State() = default;
};