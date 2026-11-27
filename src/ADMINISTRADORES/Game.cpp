#include "Game.hpp"
#include "States/LevelSelectionMenu.hpp"
#include "levels/Level1.hpp"
#include "levels/Level_present/level2.hpp"
#include "levels/FutureLevel1.hpp"
#include "State.hpp"
#include "States/Menu.hpp"
#include <utility>
#include "item/Weapon.hpp"
#include "Player.hpp"

Game::Game() 
{
    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode::getDesktopMode(),
        GameConfig::Window::Title,
        sf::State::Fullscreen);
    window->setFramerateLimit(GameConfig::Window::FrameRateLimit);
    states.push(std::make_unique<Menu>(window.get(), this));
    weaponProgress[0].unlocked = true;
    syncActiveWeaponLoadout();
    syncActiveCharacterBonuses();

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
    pendingTransition = [this]()
    {
        states.clear();
        hasActiveEra = false;
        pushState(std::make_unique<Menu>(window.get(), this));
    };
}

void Game::retryCurrentLevel()
{
    pendingTransition = [this]()
    {
        // Quitar LevelResultState y el nivel actual
        popState();
        popState();
        hasActiveEra = false;
        startLevel(lastPlayedEra, lastPlayedLevelNumber);
    };
}

void Game::startNextLevel()
{
    pendingTransition = [this]()
    {
        // Quitar LevelResultState y el nivel completado (queda LevelSelectionMenu debajo)
        popState();
        popState();
        hasActiveEra = false;

        const auto* nextLevel = levelTree.get_next_level(lastPlayedEra);
        if (nextLevel != nullptr)
        {
            startLevel(lastPlayedEra, nextLevel->get_key().number);
        }
        // Si el siguiente nivel aún no existe en startLevel() o ya toca minijefe,
        // se queda en LevelSelectionMenu mostrando el nuevo nodo desbloqueado.
    };
}

bool Game::startNewGame()
{
    if (hasActiveEra)
    {
        return false;
    }

    pushState(
        std::make_unique<LevelSelectionMenu>(
            window.get(),
            this
        )
    );

    return true;
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

    // Los niveles disponibles se conectan explícitamente aquí.
    if (era == LevelEra::Past && number == 1)
    {
        lastPlayedEra = era;
        lastPlayedLevelNumber = number;
        pushState(std::make_unique<Level1>(window.get(), this));
        return true;
    }

    if (era == LevelEra::Present && number == 1)
    {
        lastPlayedEra = era;
        lastPlayedLevelNumber = number;
        pushState(std::make_unique<Level2>(window.get(), this));
        return true;
    }

    if (era == LevelEra::Future && number == 1)
    {
        lastPlayedEra = era;
        lastPlayedLevelNumber = number;
        pushState(std::make_unique<FutureLevel1>(window.get(), this));
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
        if (pendingTransition)
        {
            auto action = std::move(pendingTransition);
            pendingTransition = nullptr;
            action();
        }
    }
}

void Game::syncActiveWeaponLoadout() const
{
    const auto& def = GameConfig::WeaponShop::Catalog[equippedWeaponIndex];
    WeaponLoadout loadout;
    loadout.imagePath = def.imagePath;
    loadout.damage = static_cast<float>(getWeaponDamage(equippedWeaponIndex));
    loadout.magazineCapacity = getWeaponMagazine(equippedWeaponIndex);
    loadout.fireInterval = def.fireInterval;
    Weapon::setActiveLoadout(loadout);
}

void Game::addUpgradeXp(int amount)
{
    if (amount > 0)
    {
        upgradeXp += amount;
    }
}

int Game::getUpgradeXp() const
{
    return upgradeXp;
}

std::size_t Game::getEquippedWeaponIndex() const
{
    return equippedWeaponIndex;
}

const WeaponProgress& Game::getWeaponProgress(std::size_t index) const
{
    return weaponProgress[index];
}

int Game::getWeaponDamage(std::size_t index) const
{
    const auto& def = GameConfig::WeaponShop::Catalog[index];
    return def.baseDamage + (weaponProgress[index].damageLevel - 1) * def.damagePerLevel;
}

int Game::getWeaponMagazine(std::size_t index) const
{
    const auto& def = GameConfig::WeaponShop::Catalog[index];
    return def.baseMagazine + (weaponProgress[index].ammoLevel - 1) * def.magazinePerLevel;
}

int Game::getDamageUpgradeCost(std::size_t index) const
{
    const auto& def = GameConfig::WeaponShop::Catalog[index];
    return def.baseDamageUpgradeCost +
           (weaponProgress[index].damageLevel - 1) * def.damageUpgradeCostStep;
}

int Game::getAmmoUpgradeCost(std::size_t index) const
{
    const auto& def = GameConfig::WeaponShop::Catalog[index];
    return def.baseAmmoUpgradeCost +
           (weaponProgress[index].ammoLevel - 1) * def.ammoUpgradeCostStep;
}

bool Game::buyOrEquipWeapon(std::size_t index)
{
    if (index >= GameConfig::WeaponShop::CatalogSize)
    {
        return false;
    }

    if (!weaponProgress[index].unlocked)
    {
        const int cost = GameConfig::WeaponShop::Catalog[index].unlockCost;
        if (upgradeXp < cost)
        {
            return false;
        }
        upgradeXp -= cost;
        weaponProgress[index].unlocked = true;
    }

    equippedWeaponIndex = index;
    syncActiveWeaponLoadout();
    return true;
}

bool Game::upgradeWeaponDamage(std::size_t index)
{
    if (index >= GameConfig::WeaponShop::CatalogSize ||
        !weaponProgress[index].unlocked ||
        weaponProgress[index].damageLevel >= GameConfig::WeaponShop::MaxUpgradeLevel)
    {
        return false;
    }

    const int cost = getDamageUpgradeCost(index);
    if (upgradeXp < cost)
    {
        return false;
    }

    upgradeXp -= cost;
    ++weaponProgress[index].damageLevel;
    if (index == equippedWeaponIndex)
    {
        syncActiveWeaponLoadout();
    }
    return true;
}

bool Game::upgradeWeaponAmmo(std::size_t index)
{
    if (index >= GameConfig::WeaponShop::CatalogSize ||
        !weaponProgress[index].unlocked ||
        weaponProgress[index].ammoLevel >= GameConfig::WeaponShop::MaxUpgradeLevel)
    {
        return false;
    }

    const int cost = getAmmoUpgradeCost(index);
    if (upgradeXp < cost)
    {
        return false;
    }

    upgradeXp -= cost;
    ++weaponProgress[index].ammoLevel;
    if (index == equippedWeaponIndex)
    {
        syncActiveWeaponLoadout();
    }
    return true;
}

void Game::syncActiveCharacterBonuses() const
{
    PlayerUpgradeBonuses bonuses;
    bonuses.bonusMaxHp = getBonusMaxHp();
    bonuses.bonusSpeed = getBonusSpeed();
    Player::setActiveUpgradeBonuses(bonuses);
}

const CharacterProgress& Game::getCharacterProgress() const
{
    return characterProgress;
}

int Game::getBonusMaxHp() const
{
    return (characterProgress.healthLevel - 1) *
           GameConfig::CharacterUpgrades::BonusHpPerLevel;
}

float Game::getBonusSpeed() const
{
    return static_cast<float>(characterProgress.speedLevel - 1) *
           GameConfig::CharacterUpgrades::BonusSpeedPerLevel;
}

int Game::getHealthUpgradeCost() const
{
    return GameConfig::CharacterUpgrades::BaseHpUpgradeCost +
           (characterProgress.healthLevel - 1) *
               GameConfig::CharacterUpgrades::HpUpgradeCostStep;
}

int Game::getSpeedUpgradeCost() const
{
    return GameConfig::CharacterUpgrades::BaseSpeedUpgradeCost +
           (characterProgress.speedLevel - 1) *
               GameConfig::CharacterUpgrades::SpeedUpgradeCostStep;
}

bool Game::upgradeCharacterHealth()
{
    if (characterProgress.healthLevel >= GameConfig::CharacterUpgrades::MaxUpgradeLevel)
    {
        return false;
    }

    const int cost = getHealthUpgradeCost();
    if (upgradeXp < cost)
    {
        return false;
    }

    upgradeXp -= cost;
    ++characterProgress.healthLevel;
    syncActiveCharacterBonuses();
    return true;
}

bool Game::upgradeCharacterSpeed()
{
    if (characterProgress.speedLevel >= GameConfig::CharacterUpgrades::MaxUpgradeLevel)
    {
        return false;
    }

    const int cost = getSpeedUpgradeCost();
    if (upgradeXp < cost)
    {
        return false;
    }

    upgradeXp -= cost;
    ++characterProgress.speedLevel;
    syncActiveCharacterBonuses();
    return true;
}