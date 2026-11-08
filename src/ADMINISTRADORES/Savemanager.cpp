#include "Savemanager.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

bool SaveManager::saveGame(const SaveData& data, const std::string& filepath) {
    try {
        std::filesystem::path path(filepath);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }

        nlohmann::json j;
        j["currentEra"] = data.currentEra;
        j["levelId"] = data.levelId;
        j["characterId"] = data.characterId;
        j["stats"] = {
            {"currentHp", data.currentHp},
            {"maxHp", data.maxHp},
            {"playerLevel", data.playerLevel},
            {"exp", data.exp}
        };
        j["position"] = {
            {"x", data.position.x},
            {"y", data.position.y}
        };

        nlohmann::json bossesJson = nlohmann::json::array();
        for (size_t i = 0; i < data.defeatedBosses.size(); ++i) {
            bossesJson.push_back(data.defeatedBosses[i]);
        }
        j["defeatedBosses"] = bossesJson;

        nlohmann::json invJson = nlohmann::json::array();
        for (size_t i = 0; i < data.inventory.size(); ++i) {
            invJson.push_back(data.inventory[i]);
        }
        j["inventory"] = invJson;

        std::ofstream file(filepath);
        if (!file.is_open()) return false;

        file << j.dump(4);
        std::cout << "[SaveManager] Partida guardada con DeSiGNAR en " << filepath << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Error al guardar: " << e.what() << "\n";
        return false;
    }
}

bool SaveManager::loadGame(SaveData& outData, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "[SaveManager] No existe partida guardada en " << filepath << "\n";
            return false;
        }

        nlohmann::json j;
        file >> j;

        outData.currentEra = j.value("currentEra", "Edad_Media");
        outData.levelId = j.value("levelId", 1);
        outData.characterId = j.value("characterId", "heroe");

        if (j.contains("stats")) {
            outData.currentHp = j["stats"].value("currentHp", 250);
            outData.maxHp = j["stats"].value("maxHp", 250);
            outData.playerLevel = j["stats"].value("playerLevel", 1);
            outData.exp = j["stats"].value("exp", 0);
        }

        if (j.contains("position")) {
            outData.position.x = j["position"].value("x", 200.f);
            outData.position.y = j["position"].value("y", 500.f);
        }

        outData.defeatedBosses.clear();
        if (j.contains("defeatedBosses") && j["defeatedBosses"].is_array()) {
            for (const auto& item : j["defeatedBosses"]) {
                outData.defeatedBosses.append(item.get<std::string>());
            }
        }

        outData.inventory.clear();
        if (j.contains("inventory") && j["inventory"].is_array()) {
            for (const auto& item : j["inventory"]) {
                outData.inventory.append(item.get<std::string>());
            }
        }

        std::cout << "[SaveManager] Partida cargada con DeSiGNAR: Era " << outData.currentEra
                  << " | HP: " << outData.currentHp << "/" << outData.maxHp
                  << " | Items en inventario: " << outData.inventory.size() << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Error al cargar JSON: " << e.what() << "\n";
        return false;
    }
}

bool SaveManager::hasSaveFile(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}