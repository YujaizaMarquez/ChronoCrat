#pragma once

#include "Bullet.hpp"
#include "Enemy.hpp"
#include "EnemyFactory.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "levels/LevelState.hpp"

#include <SFML/Graphics.hpp>
#include <array.hpp>
#include <memory>
#include <optional>

class Game;

class Level2 : public LevelState
{
public:
    static constexpr float MinWalkableY = 870.f;
    static constexpr float MaxWalkableY = 1080.f;

    Level2(sf::RenderWindow* window, Game* game);

protected:
    sf::Vector2f getPlayerPosition() const override;

    void handleLevelEvent(sf::Event& event) override;

    void updateLevel(
        float deltaTime,
        bool movementBlocked,
        std::optional<LevelArenaBounds> activeArenaBounds) override;

    void drawWorld() override;
    void drawHud() override;

    void spawnEncounter(
        int encounterIndex,
        const LevelEncounter& encounter) override;

private:
    struct EnteringEnemy
    {
        Enemy* enemy = nullptr;
        sf::Vector2f target;
    };

    Player player;


    sf::Texture tapabocasTexture;
sf::Texture tuboVirusTexture;

std::optional<sf::Sprite> tapabocasSprite;
std::optional<sf::Sprite> tuboVirusSprite;

int tapabocasFrame = 0;
int tuboVirusFrame = 0;

float tapabocasAnimationTimer = 0.f;
float tuboVirusAnimationTimer = 0.f;

static constexpr int TotalObjectFrames = 5;
static constexpr float ObjectAnimationSpeed = 0.10f;

void initializeAnimatedObjects();
void updateAnimatedObjects(float deltaTime);
void drawAnimatedObjects();

    // Científico NPC que se puede rescatar.
    Entity cientifico1;
    bool cientifico1Loaded = false;
    bool cientifico1RescuedOrReached = false;

    EnemyFactory enemyFactory;

    Designar::DynArray<std::unique_ptr<Enemy>> enemies;
    Designar::DynArray<EnteringEnemy> enteringEnemies;
    Designar::DynArray<Bullet> playerBullets;

    Designar::DynArray<sf::FloatRect> movementCollisionAreas;
    Designar::DynArray<sf::FloatRect> projectileCollisionAreas;

    sf::FloatRect worldBounds;

    int activeEncounterIndex = -1;
    bool playerDefeated = false;

    void initializeCollisionAreas();

    void updatePlayerWeapon(float deltaTime);
    void updatePlayerBullets(float deltaTime);
    void updateCientifico1(float deltaTime);

    bool collidesAt(const sf::Vector2f& position) const;

    void resolveMovement(
        Entity& entity,
        const sf::Vector2f& previousPosition,
        const std::optional<LevelArenaBounds>& activeArenaBounds);

    void resolveCientifico1Movement(
        const sf::Vector2f& previousPosition);
};