#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
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
        bool unlockAllLevels = false;
        LevelEra activeEra = LevelEra::Past;
        LevelEra lastPlayedEra = LevelEra::Past;
        int lastPlayedLevelNumber = 1;
        std::function<void()> pendingTransition;

    public:
    Game();
    ~Game();
    
    void pushState(std::unique_ptr<State> state);
    void popState();
    void returnToMainMenu();
    void retryCurrentLevel();
    void startNextLevel();
    bool startNewGame();
    void enableUnlockAllLevels();
    bool areAllLevelsUnlocked() const;
    bool startLevel(LevelEra era, int number);
    bool startMiniBoss(LevelEra era);
    bool startFinalBoss();
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