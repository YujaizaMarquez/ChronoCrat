
#include "Game.hpp"
#include "GameConfig.hpp"
#include "States/LevelSelectionMenu.hpp"
#include <cmath>
#include <stdexcept>
#include <string>

// ---------------------------------
// Constructor
// ---------------------------------

LevelSelectionMenu::LevelSelectionMenu(
    sf::RenderWindow* window,
    Game* game
)
    : State(window, game),
      backgroundSprite(backgroundTexture)
{
    if (!backgroundTexture.loadFromFile(GameConfig::LevelMap::BackgroundPath))
    {
        throw std::runtime_error(
            std::string("No se pudo cargar ") + GameConfig::LevelMap::BackgroundPath
        );
    }

    const sf::Vector2u textureSize = backgroundTexture.getSize();
    if (textureSize.x !=
        GameConfig::LevelMap::FrameWidth * GameConfig::LevelMap::FrameCount)
    {
        throw std::runtime_error(
            "El ancho del spritesheet del mapa debe ser exactamente " +
            std::to_string(
                GameConfig::LevelMap::FrameWidth *
                GameConfig::LevelMap::FrameCount) +
            " pixeles"
        );
    }

    backgroundTexture.setSmooth(false);
    backgroundSprite.setTextureRect(
        sf::IntRect(
            {0, 0},
            {GameConfig::LevelMap::FrameWidth, static_cast<int>(textureSize.y)}
        )
    );
    updateBackgroundScale();
    initializeTree();
    updateNodeColors();
}

void LevelSelectionMenu::updateBackgroundScale()
{
    const sf::Vector2u textureSize = backgroundTexture.getSize();
    const sf::Vector2u windowSize = window->getSize();

    backgroundSprite.setScale({
        static_cast<float>(windowSize.x) / GameConfig::LevelMap::FrameWidth,
        static_cast<float>(windowSize.y) / textureSize.y
    });
}

// ---------------------------------
// Crear y posicionar los círculos
// ---------------------------------

void LevelSelectionMenu::initializeTree()
{
    const sf::Vector2u size = window->getSize();

    const float width = static_cast<float>(size.x);
    const float height = static_cast<float>(size.y);

    // RAÍZ: jefe final.
    root.setRadius(35.f);
    root.setOrigin({35.f, 35.f});
    root.setPosition({width * 0.5f, height * 0.16f});

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

        // Jefe de cada era.
        eras[i].setRadius(29.f);
        eras[i].setOrigin({29.f, 29.f});
        eras[i].setPosition(eraPositions[i]);

        // Nivel 1.
        selectableLevels[i].setRadius(13.f);
        selectableLevels[i].setOrigin({13.f, 13.f});
        selectableLevels[i].setPosition(selectablePositions[i]);

        // Nivel 2.
        lockedLevels[i].setRadius(13.f);
        lockedLevels[i].setOrigin({13.f, 13.f});
        lockedLevels[i].setPosition(lockedPositions[i]);
    }
}

// ---------------------------------
// Actualizar colores según el árbol
// ---------------------------------

void LevelSelectionMenu::updateNodeColors()
{
    if (game == nullptr)
        return;

    const LevelEra erasList[] = {
        LevelEra::Past,
        LevelEra::Present,
        LevelEra::Future
    };

    // El jefe final se habilita cuando se completan
    // los tres jefes de las eras.
    if (game->getLevelTree().can_fight_final_boss())
        root.setFillColor(sf::Color::Green);
    else
        root.setFillColor(sf::Color(70, 70, 70));

    for (int i = 0; i < 3; i++)
    {
        LevelEra era = erasList[i];

        // Consultamos el siguiente nivel pendiente.
        const auto* nextLevel =
            game->getLevelTree().get_next_level(era);

        // Nivel 1 disponible.
        bool level1Unlocked =
            nextLevel != nullptr &&
            nextLevel->get_key().number == 1;

        // Nivel 2 disponible.
        bool level2Unlocked =
            nextLevel != nullptr &&
            nextLevel->get_key().number == 2;

        // Jefe de era disponible.
        bool bossUnlocked =
            game->getLevelTree().can_fight_mini_boss(era);

        selectableLevels[i].setFillColor(
            level1Unlocked
                ? sf::Color::Green
                : sf::Color(70, 70, 70)
        );

        lockedLevels[i].setFillColor(
            level2Unlocked
                ? sf::Color::Green
                : sf::Color(70, 70, 70)
        );

        eras[i].setFillColor(
            bossUnlocked
                ? sf::Color::Green
                : sf::Color(70, 70, 70)
        );
    }
}

// ---------------------------------
// Dibujar conexiones
// ---------------------------------

void LevelSelectionMenu::drawConnection(
    sf::Vector2f start,
    sf::Vector2f end
)
{
    const float dx = end.x - start.x;
    const float dy = end.y - start.y;

    const float length = std::sqrt(dx * dx + dy * dy);
    const float angle =
        std::atan2(dy, dx) * 180.f / 3.14159265f;

    sf::RectangleShape line({length, 2.f});

    line.setOrigin({0.f, 1.f});
    line.setPosition(start);
    line.setRotation(sf::degrees(angle));
    line.setFillColor(sf::Color::White);

    window->draw(line);
}

// ---------------------------------
// Detectar el nodo seleccionado
// ---------------------------------

int LevelSelectionMenu::getSelectedNode(
    sf::Vector2f mousePosition
) const
{
    // Nodo 9: jefe final.
    if (root.getGlobalBounds().contains(mousePosition))
        return 9;

    for (int i = 0; i < 3; i++)
    {
        // Nodos 6, 7 y 8: jefes de las eras.
        if (eras[i].getGlobalBounds().contains(mousePosition))
            return 6 + i;

        // Nodos 0, 1 y 2: primeros niveles.
        if (selectableLevels[i].getGlobalBounds().contains(mousePosition))
            return i;

        // Nodos 3, 4 y 5: segundos niveles.
        if (lockedLevels[i].getGlobalBounds().contains(mousePosition))
            return 3 + i;
    }

    return -1;
}

// ---------------------------------
// Actualizar efecto hover
// ---------------------------------

void LevelSelectionMenu::updateHover()
{
    const sf::Vector2i mouse =
        sf::Mouse::getPosition(*window);

    const sf::Vector2f mousePosition(
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    );

    int selected = getSelectedNode(mousePosition);

    for (int i = 0; i < 3; i++)
    {
        // Efecto para los primeros niveles.
        if (selected == i)
        {
            selectableLevels[i].setRadius(19.f);
            selectableLevels[i].setOrigin({19.f, 19.f});
        }
        else
        {
            selectableLevels[i].setRadius(13.f);
            selectableLevels[i].setOrigin({13.f, 13.f});
        }

        // Efecto para los segundos niveles.
        if (selected == 3 + i)
        {
            lockedLevels[i].setRadius(19.f);
            lockedLevels[i].setOrigin({19.f, 19.f});
        }
        else
        {
            lockedLevels[i].setRadius(13.f);
            lockedLevels[i].setOrigin({13.f, 13.f});
        }
    }
}

// ---------------------------------
// Actualizar
// ---------------------------------

void LevelSelectionMenu::update(float deltaTime)
{
    animationTimer += deltaTime;
    while (animationTimer >= GameConfig::LevelMap::FrameDuration)
    {
        animationTimer -= GameConfig::LevelMap::FrameDuration;
        currentFrame =
            (currentFrame + 1) % GameConfig::LevelMap::FrameCount;

        const sf::Vector2u textureSize = backgroundTexture.getSize();
        backgroundSprite.setTextureRect(
            sf::IntRect(
                {currentFrame * GameConfig::LevelMap::FrameWidth, 0},
                {
                    GameConfig::LevelMap::FrameWidth,
                    static_cast<int>(textureSize.y)
                }
            )
        );
    }

    updateNodeColors();
    updateHover();
}

// ---------------------------------
// Eventos del mouse
// ---------------------------------

void LevelSelectionMenu::handleEvent(sf::Event& event)
{
    if (event.is<sf::Event::Resized>())
    {
        updateBackgroundScale();
        initializeTree();
        updateNodeColors();
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

            int selected = getSelectedNode(mousePosition);

            if (game == nullptr)
                return;

            const LevelEra erasList[] = {
                LevelEra::Past,
                LevelEra::Present,
                LevelEra::Future
            };

            // Nodos 0, 1 y 2: nivel 1.
            if (selected >= 0 && selected <= 2)
            {
                game->startLevel(
                    erasList[selected],
                    1
                );
            }

            // Nodos 3, 4 y 5: nivel 2.
            else if (selected >= 3 && selected <= 5)
            {
                game->startLevel(
                    erasList[selected - 3],
                    2
                );
            }

            // Nodos 6, 7 y 8: jefes de era.
            else if (selected >= 6 && selected <= 8)
            {
                game->startMiniBoss(
                    erasList[selected - 6]
                );
            }

            // Nodo 9: jefe final.
            else if (selected == 9)
            {
                game->startFinalBoss();
            }
        }
    }
}

// ---------------------------------
// Dibujar árbol
// ---------------------------------

void LevelSelectionMenu::draw()
{
    window->draw(backgroundSprite);

    // Conexiones de la raíz con los jefes.
    for (int i = 0; i < 3; i++)
    {
        drawConnection(
            root.getPosition(),
            eraPositions[i]
        );
    }

    // Conexiones de cada jefe con sus niveles.
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

    window->draw(root);

    for (int i = 0; i < 3; i++)
    {
        window->draw(eras[i]);
        window->draw(selectableLevels[i]);
        window->draw(lockedLevels[i]);
    }
}
