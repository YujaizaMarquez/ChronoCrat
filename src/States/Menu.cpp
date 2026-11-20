#include "Game.hpp"
#include "States/Menu.hpp"
#include <stdexcept>

Menu::Menu(sf::RenderWindow* window, Game* game)
    : State(window, game),
      backgroundSprite(backgroundTexture)
{
    // 1. Cargar spritesheet del fondo
    if (!backgroundTexture.loadFromFile("assets/fondo_animado.png"))
    {
        throw std::runtime_error(
            "No se pudo cargar assets/fondo_animado.png"
        );
    }

    const sf::Vector2u textureSize = backgroundTexture.getSize();

    // Comprobar que existan los cuatro frames
    if (textureSize.x < FRAME_WIDTH * FRAME_COUNT)
    {
        throw std::runtime_error(
            "El spritesheet debe tener al menos 5120 pixeles de ancho"
        );
    }

    if (textureSize.x != FRAME_WIDTH * FRAME_COUNT)
    {
        throw std::runtime_error(
            "El ancho del spritesheet debe ser exactamente 5120 pixeles"
        );
    }

    backgroundTexture.setSmooth(false);

    // Mostrar inicialmente el primer frame
    backgroundSprite.setTextureRect(
        sf::IntRect(
            {0, 0},
            {
                FRAME_WIDTH,
                static_cast<int>(textureSize.y)
            }
        )
    );

    updateBackgroundScale();

    initializeButtons();

    // 2. Configurar botón de cerrar
    closeButton.setRadius(20.f);
    closeButton.setFillColor(sf::Color(100, 35, 35, 220));

    // Primera línea de la X
    crossLine1.setSize({20.f, 3.f});
    crossLine1.setOrigin({10.f, 1.5f});
    crossLine1.setRotation(sf::degrees(45.f));
    crossLine1.setFillColor(sf::Color::White);

    // Segunda línea de la X
    crossLine2.setSize({20.f, 3.f});
    crossLine2.setOrigin({10.f, 1.5f});
    crossLine2.setRotation(sf::degrees(-45.f));
    crossLine2.setFillColor(sf::Color::White);

    updateClosePosition();
}


// Ajustar fondo al tamaño de la ventana
void Menu::updateBackgroundScale()
{
    const sf::Vector2u textureSize = backgroundTexture.getSize();
    const sf::Vector2u windowSize = window->getSize();

    backgroundSprite.setScale({
        static_cast<float>(windowSize.x) / FRAME_WIDTH,
        static_cast<float>(windowSize.y) / textureSize.y
    });      
}


// Posicionar la X en la esquina superior derecha
void Menu::updateClosePosition()
{
    const sf::Vector2u windowSize = window->getSize();

    const float x = static_cast<float>(windowSize.x) - 55.f;
    const float y = 15.f;

    closeButton.setPosition({x, y});

    crossLine1.setPosition({x + 20.f, y + 20.f});
    crossLine2.setPosition({x + 20.f, y + 20.f});

    closeBounds = sf::FloatRect(
        {x, y},
        {40.f, 40.f}
    );
}


// Actualizar animación y botón
void Menu::update(float deltaTime)
{
    // Animación del fondo
    animationTimer += deltaTime;

    if (animationTimer >= FRAME_DURATION)
    {
        animationTimer -= FRAME_DURATION;

        currentFrame = (currentFrame + 1) % FRAME_COUNT;

        const sf::Vector2u textureSize = backgroundTexture.getSize();

        backgroundSprite.setTextureRect(
            sf::IntRect(
                {currentFrame * FRAME_WIDTH, 0},
                {
                    FRAME_WIDTH,
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
            sf::Color(200, 55, 55, 240)
        );
    }
    else
    {
        closeButton.setFillColor(
            sf::Color(100, 35, 35, 220)
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
    const float scaleX = static_cast<float>(windowSize.x) / 1920.f;
    const float scaleY = static_cast<float>(windowSize.y) / 1080.f;
    const sf::Vector2f positions[] = {
        {686.f * scaleX, 509.f * scaleY},
        {686.f * scaleX, 643.f * scaleY},
        {686.f * scaleX, 772.f * scaleY}
    };

    const sf::Vector2f sizes[] = {
        {560.f * scaleX, 85.f * scaleY},
        {560.f * scaleX, 83.f * scaleY},
        {560.f * scaleX, 92.f * scaleY}
    };

    for (int i = 0; i < 3; i++)
    {
        buttons[i].setPosition(positions[i]);
        buttons[i].setSize(sizes[i]);

        // Interior transparente
        buttons[i].setFillColor(sf::Color::Transparent);

        // Contorno negro
        buttons[i].setOutlineColor(sf::Color::Black);
        buttons[i].setOutlineThickness(2.f);
    }
}
int Menu::getSelectedButton(sf::Vector2f mousePosition) const
{
    for (int i = 0; i < 3; i++)
    {
        if (buttons[i].getGlobalBounds().contains(mousePosition))
        {
            return i;
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
    for (int i = 0; i < 3; i++)
    {
        window->draw(buttons[i]);
    }

    window->draw(closeButton);
    window->draw(crossLine1);
    window->draw(crossLine2);
}
