#pragma once
#include "State.hpp"

class Menu : public State
{
    private:
        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite;

    public:
        Menu(sf::RenderWindow* window, Game* game);

        void update(float deltaTime) override;
        void handleEvent(sf::Event& event) override;
        void draw() override;
};