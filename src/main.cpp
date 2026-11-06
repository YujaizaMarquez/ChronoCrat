#include "Game.hpp"
#include <exception>
#include <iostream>

int main()
{
    try {
        Game game;
        game.run();
    } catch (const std::exception& error) {
        std::cerr << "Error al iniciar el juego: " << error.what() << '\n';
        return 1;
    }

    return 0;
}