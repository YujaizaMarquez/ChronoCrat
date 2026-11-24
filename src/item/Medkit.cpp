#include "item/Medkit.hpp"
#include <stdexcept>

namespace
{
sf::Image createProceduralMedkitImage()
{
    constexpr unsigned int width = 32;
    constexpr unsigned int height = 24;
    sf::Image image({width, height}, sf::Color::Transparent);

    const sf::Color outline(45, 50, 58);
    const sf::Color body(242, 244, 248);
    const sf::Color shade(208, 214, 222);
    const sf::Color crossRed(215, 42, 42);

    // Asa superior
    for (unsigned int y = 0; y <= 3; ++y)
    {
        for (unsigned int x = 11; x <= 20; ++x)
        {
            const bool isInnerHole = (y >= 2 && x >= 13 && x <= 18);
            if (!isInnerHole)
            {
                image.setPixel({x, y}, outline);
            }
        }
    }

    // Caja blanca con borde oscuro y sombreado inferior
    for (unsigned int y = 4; y <= 23; ++y)
    {
        for (unsigned int x = 2; x <= 29; ++x)
        {
            const bool isBorder =
                (x <= 3 || x >= 28 || y <= 5 || y >= 22);
            if (isBorder)
            {
                image.setPixel({x, y}, outline);
            }
            else if (y >= 20)
            {
                image.setPixel({x, y}, shade);
            }
            else
            {
                image.setPixel({x, y}, body);
            }
        }
    }

    // Cruz roja centrada
    for (unsigned int y = 8; y <= 19; ++y)
    {
        for (unsigned int x = 14; x <= 17; ++x)
        {
            image.setPixel({x, y}, crossRed);
        }
    }
    for (unsigned int y = 12; y <= 15; ++y)
    {
        for (unsigned int x = 10; x <= 21; ++x)
        {
            image.setPixel({x, y}, crossRed);
        }
    }

    return image;
}
}

Medkit::Medkit(const std::string& imagePath, int healAmount)
    : Item(
          ItemType::HealthPotion,
          static_cast<float>(healAmount),
          0.f,
          GameConfig::Medkit::ImageWidth,
          GameConfig::Medkit::ImageHeight),
      healAmount(healAmount)
{
    bool loaded = false;
    if (!imagePath.empty())
    {
        loaded = loadSpriteSheet(imagePath, false);
    }

    if (!loaded && !loadFromImage(createProceduralMedkitImage()))
    {
        throw std::runtime_error("No se pudo inicializar el grafico del botiquin");
    }

    setVisualScale(GameConfig::Medkit::VisualScale);
}

void Medkit::use()
{
    consumed = true;
}

int Medkit::getHealAmount() const
{
    return healAmount;
}

bool Medkit::isConsumed() const
{
    return consumed;
}