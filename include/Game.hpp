#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "State.hpp"
#include "stack.hpp"

using namespace Designar;

class Game
{
    private:
        std::unique_ptr<sf::RenderWindow> window;
        DynStack<std::unique_ptr<State>> states;
    public:
    Game();
    ~Game();
    
    void pushState(std::unique_ptr<State> state);
    void popState();
    void run();
};