#pragma once
#include "Entity.hpp"
#include <array.hpp>
#include <string>

class Player : public Entity
{
private:
    int playerLevel;
    int exp;
    Designar::DynArray<std::string> inventory;
    Designar::DynArray<std::string> defeatedBosses;

public:
    Player();
    ~Player() override = default;
    
    void handleInput(float deltaTime);
    void update(float deltaTime) override;

    // Progreso y estadísticas
    void addExp(int amount);
    int getPlayerLevel() const;
    int getExp() const;

    // Gestión de inventario con DeSiGNAR
    void addItem(const std::string& item);
    bool hasItem(const std::string& item) const;
    void clearInventory();
    const Designar::DynArray<std::string>& getInventory() const;

    // Registro de jefes derrotados con DeSiGNAR
    void addDefeatedBoss(const std::string& bossId);
    bool hasDefeatedBoss(const std::string& bossId) const;
    const Designar::DynArray<std::string>& getDefeatedBosses() const;

    // Conexión con SaveData y SaveManager
    SaveData exportSaveData(const std::string& currentEra, int levelId) const;
    void importSaveData(const SaveData& data);
};