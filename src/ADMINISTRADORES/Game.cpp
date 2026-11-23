#include "Game.hpp"
#include "Savemanager.hpp"
#include "States/LevelSelectionMenu.hpp"
#include "levels/Level1.hpp"
#include "levels/Level2.hpp"
#include "State.hpp"
#include "States/Menu.hpp"
#include <utility>
#include <string>

namespace
{
std::string eraToString(LevelEra era)
{
    switch (era)
    {
        case LevelEra::Past: return "Past";
        case LevelEra::Present: return "Present";
        case LevelEra::Future: return "Future";
        default: return "None";
    }
}
}

Game::Game() 
{
    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode::getDesktopMode(),
        GameConfig::Window::Title,
        sf::State::Fullscreen);
    window->setFramerateLimit(GameConfig::Window::FrameRateLimit);
    states.push(std::make_unique<Menu>(window.get(), this));
}

Game::~Game() {}

void Game::pushState(std::unique_ptr<State> state) {
    states.push(std::move(state));
}

void Game::popState() {
    if (!states.is_empty()) {
        states.pop();
    }
}

void Game::returnToMainMenu()
{
    states.clear();
    hasActiveEra = false;
    pushState(std::make_unique<Menu>(window.get(), this));
}

bool Game::startNewGame()
{
    if (hasActiveEra)
    {
        return false;
    }

    levelTree.reset();

    pushState(
        std::make_unique<LevelSelectionMenu>(
            window.get(),
            this
        )
    );

    return true;
}


bool Game::continueGame()
{
    if (hasActiveEra)
    {
        return false;
    }

    SaveData data;
    if (!SaveManager::loadGame(data))
    {
        return false;
    }

    levelTree.import_progress(data.completedNodes);
    pushState(std::make_unique<LevelSelectionMenu>(window.get(), this));
    return true;
}

bool Game::saveProgress()
{
    // Bloqueo explícito: no se permite guardar mientras se está jugando un nivel
    if (hasActiveEra)
    {
        return false;
    }

    SaveData data;
    data.currentEra = eraToString(activeEra);
    data.levelId = 0;

    const Designar::DynArray<std::string> progress = levelTree.export_progress();
    for (std::size_t i = 0; i < progress.size(); ++i)
    {
        data.completedNodes.append(progress[i]);
    }

    return SaveManager::saveGame(data);
}

bool Game::hasSaveFile() const
{
    return SaveManager::hasSaveFile();
}


bool Game::startLevel(LevelEra era, int number)
{
    // Comprobar que el nivel solicitado esté desbloqueado.
    const auto* nextLevel = levelTree.get_next_level(era);

    if (nextLevel == nullptr ||
        nextLevel->get_key().number != number)
    {
        return false;
    }

    // Seleccionar la era.
    if (!selectEra(era))
        return false;

    // Por ahora solo tenemos conectados estos dos escenarios.
    if (era == LevelEra::Past && number == 1)
    {
        pushState(std::make_unique<Level1>(window.get(), this));
        return true;
    }

    if (era == LevelEra::Present && number == 1)
    {
        pushState(std::make_unique<Level2>(window.get(), this));
        return true;
    }

    // Los otros escenarios se conectarán cuando existan sus clases.
    // No marcamos como iniciado un escenario que todavía no está implementado.
    hasActiveEra = false;

    return false;
}

bool Game::startMiniBoss(LevelEra era)
{
    if (!levelTree.can_fight_mini_boss(era))
        return false;

    // Todavía falta conectar las clases de los tres jefes.
    return false;
}

bool Game::startFinalBoss()
{
    if (!canFightFinalBoss())
        return false;

    // Todavía falta conectar la clase del jefe final.
    return false;
}


bool Game::selectEra(LevelEra era)
{
    if (hasActiveEra || levelTree.get_mini_boss(era) == nullptr ||
        levelTree.is_era_completed(era) ||
        levelTree.get_root()->get_key().completed)
    {
        return false;
    }

    activeEra = era;
    hasActiveEra = true;
    return true;
}

const Designar::MTreeNode<LevelInfo>* Game::getCurrentLevel() const
{
    return hasActiveEra ? levelTree.get_next_level(activeEra) : nullptr;
}

bool Game::completeCurrentLevel()
{
    if (!hasActiveEra)
        return false;

    if (!levelTree.complete_next_level(activeEra))
        return false;

    hasActiveEra = false;
    saveProgress();

    return true;
}

bool Game::canFightCurrentMiniBoss() const
{
    return hasActiveEra && levelTree.can_fight_mini_boss(activeEra);
}

bool Game::completeCurrentMiniBoss()
{
    if (!canFightCurrentMiniBoss() || !levelTree.complete_mini_boss(activeEra))
    {
        return false;
    }

    hasActiveEra = false;
    saveProgress();
    return true;
}

bool Game::canFightFinalBoss() const
{
    return !hasActiveEra && !levelTree.get_root()->get_key().completed &&
           levelTree.can_fight_final_boss();
}

bool Game::completeFinalBoss()
{
    if (!canFightFinalBoss() || !levelTree.complete_final_boss())
    {
        return false;
    }

    saveProgress();
    return true;
}

const LevelTree& Game::getLevelTree() const
{
    return levelTree;
}

void Game::run() 
{
    sf::Clock clock;
    while (window->isOpen()) 
    {
        float deltaTime = clock.restart().asSeconds();

        // 1. Procesar eventos (solo para el estado superior)
        while (auto event = window->pollEvent()) {
            if (!states.is_empty()) {
                states.top()->handleEvent(*event);
            }

            if (event->is<sf::Event::Closed>()) {
                window->close();
            }
        }

        // 2. Actualizar (solo el estado superior)
        if (!states.is_empty()) 
        {
            states.top()->update(deltaTime);
        }

        // 3. Renderizar solo el estado superior
        window->clear(sf::Color::Black);

        if (!states.is_empty())
        {
            window->setView(window->getDefaultView());
            states.top()->draw();
        }

        window->display();
    }
}