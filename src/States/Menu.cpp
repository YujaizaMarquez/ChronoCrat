#include "Game.hpp"
#include "States/Menu.hpp"
#include <stdexcept>
#include <string>

Menu::Menu(sf::RenderWindow* window, Game* game)
    : State(window, game),
      backgroundSprite(backgroundTexture)
{
    // 1. Cargar spritesheet del fondo
    if (!backgroundTexture.loadFromFile(GameConfig::Menu::BackgroundPath))
    {
        throw std::runtime_error(
            std::string("No se pudo cargar ") + GameConfig::Menu::BackgroundPath
        );
    }

    const sf::Vector2u textureSize = backgroundTexture.getSize();

    // Comprobar que existan los cuatro frames
    if (textureSize.x <
        GameConfig::Menu::FrameWidth * GameConfig::Menu::FrameCount)
    {
        throw std::runtime_error(
            "El spritesheet debe tener al menos " +
            std::to_string(
                GameConfig::Menu::FrameWidth * GameConfig::Menu::FrameCount) +
            " pixeles de ancho"
        );
    }

    if (textureSize.x !=
        GameConfig::Menu::FrameWidth * GameConfig::Menu::FrameCount)
    {
        throw std::runtime_error(
            "El ancho del spritesheet debe ser exactamente " +
            std::to_string(
                GameConfig::Menu::FrameWidth * GameConfig::Menu::FrameCount) +
            " pixeles"
        );
    }

    backgroundTexture.setSmooth(false);

    // Mostrar inicialmente el primer frame
    backgroundSprite.setTextureRect(
        sf::IntRect(
            {0, 0},
            {
                GameConfig::Menu::FrameWidth,
                static_cast<int>(textureSize.y)
            }
        )
    );

    updateBackgroundScale();

    initializeButtons();

    // 2. Configurar botón de cerrar
    closeButton.setRadius(GameConfig::Menu::CloseButtonRadius);
    closeButton.setFillColor(GameConfig::Menu::CloseButtonColor);

    // Primera línea de la X
    crossLine1.setSize({
        GameConfig::Menu::CloseCrossSize,
        GameConfig::Menu::CloseCrossThickness
    });
    crossLine1.setOrigin({
        GameConfig::Menu::CloseCrossOriginX,
        GameConfig::Menu::CloseCrossOriginY
    });
    crossLine1.setRotation(sf::degrees(GameConfig::Menu::CloseCrossAngle));
    crossLine1.setFillColor(sf::Color::White);

    // Segunda línea de la X
    crossLine2.setSize({
        GameConfig::Menu::CloseCrossSize,
        GameConfig::Menu::CloseCrossThickness
    });
    crossLine2.setOrigin({
        GameConfig::Menu::CloseCrossOriginX,
        GameConfig::Menu::CloseCrossOriginY
    });
    crossLine2.setRotation(sf::degrees(-GameConfig::Menu::CloseCrossAngle));
    crossLine2.setFillColor(sf::Color::White);

    updateClosePosition();
}


// Ajustar fondo al tamaño de la ventana
void Menu::updateBackgroundScale()
{
    const sf::Vector2u textureSize = backgroundTexture.getSize();
    const sf::Vector2u windowSize = window->getSize();

    backgroundSprite.setScale({
        static_cast<float>(windowSize.x) / GameConfig::Menu::FrameWidth,
        static_cast<float>(windowSize.y) / textureSize.y
    });      
}


// Posicionar la X en la esquina superior derecha
void Menu::updateClosePosition()
{
    const sf::Vector2u windowSize = window->getSize();

    const float x = static_cast<float>(windowSize.x) -
                    GameConfig::Menu::CloseButtonRightOffset;
    const float y = GameConfig::Menu::CloseButtonTopOffset;

    closeButton.setPosition({x, y});

    crossLine1.setPosition({
        x + GameConfig::Menu::CloseCrossOffset,
        y + GameConfig::Menu::CloseCrossOffset
    });
    crossLine2.setPosition({
        x + GameConfig::Menu::CloseCrossOffset,
        y + GameConfig::Menu::CloseCrossOffset
    });

    closeBounds = sf::FloatRect(
        {x, y},
        {
            GameConfig::Menu::CloseBoundsSize,
            GameConfig::Menu::CloseBoundsSize
        }
    );
}


// Actualizar animación y botón
void Menu::update(float deltaTime)
{
    // Animación del fondo
    animationTimer += deltaTime;

    if (animationTimer >= GameConfig::Menu::FrameDuration)
    {
        animationTimer -= GameConfig::Menu::FrameDuration;

        currentFrame =
            (currentFrame + 1) % GameConfig::Menu::FrameCount;

        const sf::Vector2u textureSize = backgroundTexture.getSize();

        backgroundSprite.setTextureRect(
            sf::IntRect(
                {currentFrame * GameConfig::Menu::FrameWidth, 0},
                {
                    GameConfig::Menu::FrameWidth,
                    static_cast<int>(textureSize.y)
                }
            )
        );
    }

    // Detectar si el cursor está sobre la X
    const sf::Vector2i mousePosition =
        sf::Mouse::getPosition(*window);

    mouseOverClose = closeBounds.contains(
        sf::Vector2f(
            static_cast<float>(mousePosition.x),
            static_cast<float>(mousePosition.y)
        )
    );

    // Cambiar color al pasar el cursor
    if (mouseOverClose)
    {
        closeButton.setFillColor(
            GameConfig::Menu::CloseButtonHoverColor
        );
    }
    else
    {
        closeButton.setFillColor(
            GameConfig::Menu::CloseButtonColor
        );
    }
}


// Procesar eventos
void Menu::handleEvent(sf::Event& event)
{
    // Redimensionar ventana
    if (const auto* resized =
        event.getIf<sf::Event::Resized>())
    {
        updateBackgroundScale();
        updateClosePosition();
        initializeButtons();
    }

    if (const auto* mouseEvent =
        event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseEvent->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f mousePosition = {
                static_cast<float>(mouseEvent->position.x),
                static_cast<float>(mouseEvent->position.y)
            };

            if (closeBounds.contains(mousePosition))
            {
                window->close();
                return;
            }

            if (getSelectedButton(mousePosition) == 0 && game != nullptr)
            {
                game->startNewGame();
            }
        }
    }
}
void Menu::initializeButtons()
{
    const sf::Vector2u windowSize = window->getSize();
    const float scaleX =
        static_cast<float>(windowSize.x) / GameConfig::Menu::ReferenceWidth;
    const float scaleY =
        static_cast<float>(windowSize.y) / GameConfig::Menu::ReferenceHeight;
    const auto configureButton = [&](std::size_t index,
                                     const GameConfig::Menu::ButtonLayout& layout)
    {
        buttons[index].setPosition({layout.x * scaleX, layout.y * scaleY});
        buttons[index].setSize({
            layout.width * scaleX,
            layout.height * scaleY
        });

        // Interior transparente
        buttons[index].setFillColor(sf::Color::Transparent);

        // Contorno negro
        buttons[index].setOutlineColor(sf::Color::Black);
        buttons[index].setOutlineThickness(
            GameConfig::Menu::ButtonOutlineThickness);
    };
    configureButton(0, GameConfig::Menu::NewGameButton);
    configureButton(1, GameConfig::Menu::ContinueButton);
    configureButton(2, GameConfig::Menu::ExitButton);
}
int Menu::getSelectedButton(sf::Vector2f mousePosition) const
{
    for (std::size_t i = 0; i < GameConfig::Menu::ButtonCount; ++i)
    {
        if (buttons[i].getGlobalBounds().contains(mousePosition))
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}

// Dibujar menú
void Menu::draw()
{
    // Fondo animado
    window->draw(backgroundSprite);
     // Dibujar los tres botones
    for (std::size_t i = 0; i < GameConfig::Menu::ButtonCount; ++i)
    {
        window->draw(buttons[i]);
    }

    window->draw(closeButton);
    window->draw(crossLine1);
    window->draw(crossLine2);
}
