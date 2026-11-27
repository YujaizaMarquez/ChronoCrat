#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "State.hpp"
#include "stack.hpp"
#include "levels/LevelTree.hpp"
#include <array>
#include <cstddef>
#include "GameConfig.hpp"

struct WeaponProgress
{
    bool unlocked = false;
    int damageLevel = 1;
    int ammoLevel = 1;
};

struct CharacterProgress
{
    int healthLevel = 1;
    int speedLevel = 1;
};

class Game
{
    private:
        std::unique_ptr<sf::RenderWindow> window;
        Designar::DynStack<std::unique_ptr<State>> states;
        LevelTree levelTree;
        bool hasActiveEra = false;
        LevelEra activeEra = LevelEra::Past;
        LevelEra lastPlayedEra = LevelEra::Past;
        int lastPlayedLevelNumber = 1;
        std::function<void()> pendingTransition;
        int upgradeXp = 0;
        std::size_t equippedWeaponIndex = 0;
        std::array<WeaponProgress, GameConfig::WeaponShop::CatalogSize> weaponProgress{};
        void syncActiveWeaponLoadout() const;
        CharacterProgress characterProgress{};
        void syncActiveCharacterBonuses() const;

    public:
    Game();
    ~Game();
    
    void pushState(std::unique_ptr<State> state);
    void popState();
    void returnToMainMenu();
    void retryCurrentLevel();
    void startNextLevel();
    bool startNewGame();
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
    void addUpgradeXp(int amount);
    int getUpgradeXp() const;
    std::size_t getEquippedWeaponIndex() const;
    const WeaponProgress& getWeaponProgress(std::size_t index) const;
    int getWeaponDamage(std::size_t index) const;
    int getWeaponMagazine(std::size_t index) const;
    int getDamageUpgradeCost(std::size_t index) const;
    int getAmmoUpgradeCost(std::size_t index) const;
    bool buyOrEquipWeapon(std::size_t index);
    bool upgradeWeaponDamage(std::size_t index);
    bool upgradeWeaponAmmo(std::size_t index);
    const CharacterProgress& getCharacterProgress() const;
    int getBonusMaxHp() const;
    float getBonusSpeed() const;
    int getHealthUpgradeCost() const;
    int getSpeedUpgradeCost() const;
    bool upgradeCharacterHealth();
    bool upgradeCharacterSpeed();
};