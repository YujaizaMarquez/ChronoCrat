#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "State.hpp"
#include "stack.hpp"
#include "levels/LevelTree.hpp"

class Game
{
    private:
        std::unique_ptr<sf::RenderWindow> window;
        Designar::DynStack<std::unique_ptr<State>> states;
        LevelTree levelTree;
        bool hasActiveEra = false;
        LevelEra activeEra = LevelEra::Past;
    public:
    Game();
    ~Game();
    
    void pushState(std::unique_ptr<State> state);
    void popState();
    bool selectEra(LevelEra era);
    const Designar::MTreeNode<LevelInfo>* getCurrentLevel() const;
    bool completeCurrentLevel();
    bool canFightCurrentMiniBoss() const;
    bool completeCurrentMiniBoss();
    bool canFightFinalBoss() const;
    bool completeFinalBoss();
    const LevelTree& getLevelTree() const;
    void run();
};