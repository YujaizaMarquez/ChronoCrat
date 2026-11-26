#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Mask
{
public:
    Mask();

    bool loadFromFile(
        const std::string& path,
        const sf::Vector2f& position,
        const sf::Vector2f& scale);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    sf::Vector2f getPosition() const;

private:
    sf::Texture texture;
    sf::Sprite sprite;
    int currentFrame = 0;
    float animationTimer = 0.f;

    static constexpr int FrameWidth = 20;
    static constexpr int FrameHeight = 20;
    static constexpr int FrameCount = 5;
    static constexpr float FrameDuration = 0.1f;
};
