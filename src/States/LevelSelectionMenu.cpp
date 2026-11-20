
#include "Game.hpp"
#include "States/LevelSelectionMenu.hpp"
#include <cmath>

LevelSelectionMenu::LevelSelectionMenu(
    sf::RenderWindow* window,
    Game* game
)
    : State(window, game)
{
    initializeTree();
}

// ---------------------------------
// Crear y posicionar los círculos
// ---------------------------------

void LevelSelectionMenu::initializeTree()
{
    const sf::Vector2u size = window->getSize();

    const float width = static_cast<float>(size.x);
    const float height = static_cast<float>(size.y);

    // RAÍZ
    root.setRadius(35.f);
    root.setOrigin({35.f, 35.f});
    root.setFillColor(sf::Color::White);
    root.setPosition({width * 0.5f, height * 0.16f});

    // Posiciones de los tres hijos
    const float positionsX[] = {
        width * 0.20f,
        width * 0.50f,
        width * 0.80f
    };

    for (int i = 0; i < 3; i++)
    {
        eraPositions[i] = {
            positionsX[i],
            height * 0.48f
        };

        selectablePositions[i] = {
            positionsX[i] - width * 0.055f,
            height * 0.78f
        };

        lockedPositions[i] = {
            positionsX[i] + width * 0.055f,
            height * 0.78f
        };

        // Hijos de la raíz
        eras[i].setRadius(29.f);
        eras[i].setOrigin({29.f, 29.f});
        eras[i].setFillColor(sf::Color::White);
        eras[i].setPosition(eraPositions[i]);

        // Nietos izquierdos: seleccionables
        selectableLevels[i].setRadius(13.f);
        selectableLevels[i].setOrigin({13.f, 13.f});
        selectableLevels[i].setFillColor(sf::Color::White);
        selectableLevels[i].setPosition(selectablePositions[i]);

        // Nietos derechos: no seleccionables
        lockedLevels[i].setRadius(13.f);
        lockedLevels[i].setOrigin({13.f, 13.f});
        lockedLevels[i].setFillColor(sf::Color::White);
        lockedLevels[i].setPosition(lockedPositions[i]);
    }
}

// ---------------------------------
// Dibujar conexiones entre nodos
// ---------------------------------

void LevelSelectionMenu::drawConnection(
    sf::Vector2f start,
    sf::Vector2f end
)
{
    const float dx = end.x - start.x;
    const float dy = end.y - start.y;

    const float length = std::sqrt(dx * dx + dy * dy);
    const float angle = std::atan2(dy, dx) * 180.f / 3.14159265f;

    sf::RectangleShape line({length, 2.f});

    line.setOrigin({0.f, 1.f});
    line.setPosition(start);
    line.setRotation(sf::degrees(angle));
    line.setFillColor(sf::Color::White);

    window->draw(line);
}

// ---------------------------------
// Detectar cursor sobre los nietos
// ---------------------------------

void LevelSelectionMenu::updateHover()
{
    const sf::Vector2i mouse =
        sf::Mouse::getPosition(*window);

    const sf::Vector2f mousePosition(
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    );

    hoveredLevel = getSelectedLevel(mousePosition);

    for (int i = 0; i < 3; i++)
    {
        if (i == hoveredLevel)
        {
            selectableLevels[i].setRadius(19.f);
            selectableLevels[i].setOrigin({19.f, 19.f});
        }
        else
        {
            selectableLevels[i].setRadius(13.f);
            selectableLevels[i].setOrigin({13.f, 13.f});
        }
    }
}

int LevelSelectionMenu::getSelectedLevel(
    sf::Vector2f mousePosition
) const
{
    for (int i = 0; i < 3; i++)
    {
        if (selectableLevels[i].getGlobalBounds().contains(mousePosition))
        {
            return i;
        }
    }

    return -1;
}

// ---------------------------------
// Actualizar
// ---------------------------------

void LevelSelectionMenu::update(float deltaTime)
{
    updateHover();
}

// ---------------------------------
// Eventos del mouse
// ---------------------------------

void LevelSelectionMenu::handleEvent(sf::Event& event)
{
    if (const auto* resized = event.getIf<sf::Event::Resized>())
    {
        initializeTree();
        return;
    }

    if (const auto* mouseEvent =
        event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseEvent->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePosition(
                static_cast<float>(mouseEvent->position.x),
                static_cast<float>(mouseEvent->position.y)
            );

            int selected = getSelectedLevel(mousePosition);

            if (selected == 0 && game != nullptr)
            {
                game->startLevel(LevelEra::Past);
            }

            // selected == 1: primer nieto de Presente.
            // selected == 2: primer nieto de Futuro.
            // Se reservan para niveles futuros.
        }
    }
}

// ---------------------------------
// Dibujar árbol
// ---------------------------------

void LevelSelectionMenu::draw()
{
    window->clear(sf::Color::Black);

    // Conectar raíz con sus tres hijos
    for (int i = 0; i < 3; i++)
    {
        drawConnection(
            root.getPosition(),
            eraPositions[i]
        );
    }

    // Conectar cada hijo con sus dos nietos
    for (int i = 0; i < 3; i++)
    {
        drawConnection(
            eraPositions[i],
            selectablePositions[i]
        );

        drawConnection(
            eraPositions[i],
            lockedPositions[i]
        );
    }

    // Dibujar raíz
    window->draw(root);

    // Dibujar hijos y nietos
    for (int i = 0; i < 3; i++)
    {
        window->draw(eras[i]);
        window->draw(selectableLevels[i]);
        window->draw(lockedLevels[i]);
    }
}
