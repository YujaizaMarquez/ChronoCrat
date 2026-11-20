#pragma once
#include <string>
#include <SFML/System/Vector2.hpp>
#include <array.hpp>
#include "json.hpp"

struct SaveData {
    std::string currentEra = "Edad_Media";
    int levelId = 1;
    std::string characterId = "heroe";
    int currentHp = 250;
    int maxHp = 250;
    int playerLevel = 1;
    int exp = 0;
    sf::Vector2f position = {200.f, 500.f};

    Designar::DynArray<std::string> defeatedBosses;
    Designar::DynArray<std::string> inventory;
};

class SaveManager {
public:
    static bool saveGame(const SaveData& data, const std::string& filepath = "saves/savegame.json");
    static bool loadGame(SaveData& outData, const std::string& filepath = "saves/savegame.json");
    static bool hasSaveFile(const std::string& filepath = "saves/savegame.json");
};