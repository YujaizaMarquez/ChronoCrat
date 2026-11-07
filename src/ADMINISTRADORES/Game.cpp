#include "Game.hpp"
#include "State.hpp"
#include "States/Menu.hpp"
#include <utility>

Game::Game() 
{
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), "ChronoCrat", sf::State::Fullscreen);
    window->setFramerateLimit(60);
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
    return hasActiveEra && levelTree.complete_next_level(activeEra);
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
    return true;
}

bool Game::canFightFinalBoss() const
{
    return !hasActiveEra && !levelTree.get_root()->get_key().completed &&
           levelTree.can_fight_final_boss();
}

bool Game::completeFinalBoss()
{
    return canFightFinalBoss() && levelTree.complete_final_boss();
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