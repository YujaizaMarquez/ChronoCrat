#pragma once

#include "GameConfig.hpp"
#include "State.hpp"
#include <array.hpp>
#include <optional>
#include <string>

struct LevelEnemySpawn
{
    std::string enemyType;
    sf::Vector2f position;
};

struct LevelArenaBounds
{
    float minimumX;
    float maximumX;
};

struct LevelEncounter
{
    enum class Type
    {
        LooseEnemies,
        Wave,
        Boss
    };

    enum class Trigger
    {
        AtPosition,
        AfterPrevious
    };

    Type type;
    Trigger trigger;
    float triggerX = 0.f;
    Designar::DynArray<LevelEnemySpawn> enemies;
    std::optional<LevelArenaBounds> arenaBounds;
};

struct LevelWorldBounds
{
    sf::Vector2f minimum;
    sf::Vector2f maximum;
};

class LevelState : public State
{
public:
    LevelState(
        sf::RenderWindow* window,
        Game* game,
        const std::string& backgroundPath,
        Designar::DynArray<LevelEncounter> encounters,
        LevelWorldBounds worldBounds,
        float cameraFollowSpeed = GameConfig::LevelState::CameraFollowSpeed);

    void update(float deltaTime) final;
    void handleEvent(sf::Event& event) final;
    void draw() final;

protected:
    virtual sf::Vector2f getPlayerPosition() const = 0;
    virtual void handleLevelEvent(sf::Event& event) = 0;
    virtual void updateLevel(
        float deltaTime,
        bool playerMovementBlocked,
        std::optional<LevelArenaBounds> activeArenaBounds) = 0;
    void triggerVictory();
    void triggerDefeat();
    virtual void drawWorld() = 0;
    virtual void drawHud() = 0;
    virtual void spawnEncounter(int encounterIndex, const LevelEncounter& encounter) = 0;

    void completeEncounter(int encounterIndex);
    bool areAllEncountersCompleted() const;
    bool completeLevelProgress();
    bool isPlayerMovementBlocked() const;
    std::optional<LevelArenaBounds> getActiveArenaBounds() const;
    const sf::View& getCameraView() const;

private:
    enum class EncounterStatus
    {
        Pending,
        Active,
        Completed
    };

    Designar::DynArray<LevelEncounter> encounters;
    Designar::DynArray<EncounterStatus> encounterStatuses;
    LevelWorldBounds worldBounds;
    sf::View cameraView;
    float cameraFollowSpeed;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    bool levelCompleted = false;
    bool resultShown = false;

    void processEncounterTriggers();
    bool canActivateEncounter(int encounterIndex) const;
    void updateCamera(float deltaTime);
    void clampCameraCenter(sf::Vector2f& center) const;
};
