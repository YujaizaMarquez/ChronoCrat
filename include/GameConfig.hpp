#pragma once

#include <array>
#include <cstddef>
#include <SFML/Graphics/Color.hpp>

namespace GameConfig
{
struct Rect
{
    float x;
    float y;
    float width;
    float height;
};

struct CollisionObstacle
{
    Rect bounds;
    bool blocksMovement;
    bool blocksProjectiles;
};

namespace Window
{
inline constexpr unsigned int FrameRateLimit = 60;
inline constexpr char Title[] = "ChronoCrat";
}

namespace Animation
{
inline constexpr int DefaultFrameWidth = 64;
inline constexpr int DefaultFrameHeight = 64;
inline constexpr float DefaultFrameDuration = 0.12f;
}

namespace Entity
{
inline constexpr char DefaultId[] = "heroe";
inline constexpr char DefaultName[] = "Heroe";
inline constexpr int DefaultHealth = 250;
inline constexpr int DefaultAttackDamage = 25;
inline constexpr float DefaultSpeed = 180.f;
inline constexpr float InitialX = 200.f;
inline constexpr float InitialY = 500.f;
}

namespace Player
{
inline constexpr char DataPath[] = "data/heroe.json";
inline constexpr int InitialLevel = 1;
inline constexpr int InitialExperience = 0;
inline constexpr int ExperiencePerLevel = 100;
inline constexpr int AttackDamage = 35;
inline constexpr float AttackRange = 65.f;
inline constexpr float AttackCooldown = 0.45f;
}

namespace Enemy
{
inline constexpr char SoldierDataPath[] = "data/enemigo.json";
inline constexpr float DetectionRange = 250.f;
inline constexpr float AttackRange = 45.f;
inline constexpr int AttackDamage = 15;
inline constexpr float AttackCooldown = 1.2f;
inline constexpr float IdleWaitTime = 1.5f;
inline constexpr int ExperienceReward = 50;
inline constexpr float PatrolArrivalDistance = 5.f;
inline constexpr float MovementEpsilon = 0.001f;
}

namespace Inventory
{
inline constexpr std::size_t Capacity = 5;
}

namespace InventoryHud
{
inline constexpr float SlotSize = 76.f;
inline constexpr float SlotGap = 10.f;
inline constexpr float BottomMargin = 24.f;
inline constexpr float DigitThickness = 2.5f;
inline const sf::Color SelectedBackground(68, 61, 43, 235);
inline const sf::Color SlotBackground(20, 24, 30, 220);
inline const sf::Color SelectedOutline(255, 205, 85);
inline const sf::Color SlotOutline(155, 165, 175);
}

namespace Item
{
inline constexpr float PickupRadius = 48.f;
inline constexpr float PickupArrivalDistance = 8.f;
inline constexpr float PickupSpeed = 360.f;
inline constexpr float BobFrequency = 3.f;
inline constexpr float BobAmplitude = 4.f;
inline constexpr unsigned char BlackMaskThreshold = 8;
}

namespace Bullet
{
inline constexpr float Radius = 12.f;
inline constexpr float Lifetime = 3.f;
inline constexpr float CollisionStep = 4.f;
inline constexpr float TargetHalfWidth = 32.f;
inline constexpr float TargetHeight = 112.f;
inline const sf::Color DefaultColor(235, 75, 55);
}

namespace Weapon
{
inline constexpr char Mp40ImagePath[] = "assets/era1/guerra/mp40.jpeg";
inline constexpr int Mp40ImageWidth = 1408;
inline constexpr int Mp40ImageHeight = 768;
inline constexpr float Mp40VisualScale = 0.08f;
inline constexpr float Durability = 100.f;
inline constexpr float Damage = 25.f;
inline constexpr int MagazineCapacity = 30;
inline constexpr float FireInterval = 0.18f;
}

namespace Soldier
{
inline constexpr float DetectionRange = 2000.f;
inline constexpr float ShotCooldown = 0.9f;
inline constexpr float ProjectileSpeed = 520.f;
inline constexpr int ProjectileDamage = 12;
inline constexpr float LaneTolerance = 10.f;
inline constexpr float MeleeRange = 140.f;
inline constexpr float FlankRadius = 108.f;
inline constexpr float FlankArrivalDistance = 8.f;
inline constexpr float FlankReachedDistance = 24.f;
inline constexpr float FullCircleRadians = 6.28318530718f;
inline constexpr float FlankStartAngleRadians = -1.57079632679f;
inline constexpr float SingleShooterDistance = 450.f;
inline constexpr float ClosestShooterDistance = 320.f;
inline constexpr float FarthestShooterDistance = 600.f;
inline constexpr float MinimumDistanceRatio = 0.7f;
inline constexpr float DefaultMinimumDistance = 180.f;
inline constexpr float MuzzleOffset = 60.f;
inline constexpr float MuzzleHeight = 64.f;
inline constexpr float SquadShotStagger = 1.f;
inline constexpr int ExperienceReward = GameConfig::Enemy::ExperienceReward;
}

namespace Level1
{
struct ArenaLayout
{
    float minimumMapX;
    float maximumMapX;
    float spawnMinimumMapX;
    float spawnMaximumMapX;
    std::size_t soldierCount;
};

inline constexpr char BackgroundPath[] = "assets/era1/guerra/germany1945.png";
inline constexpr float MapImageWidth = 6400.f;
inline constexpr float MapImageHeight = 704.f;
inline constexpr float PlayerStartMapX = 150.f;
inline constexpr float PlayerStartMapY = 500.f;
inline constexpr float PlayerVisualScale = 2.5f;
inline constexpr float EnemyVisualScale = 2.5f;
inline constexpr int PlayerAttackDamage = 35;
inline constexpr float PlayerAttackRange = 130.f;
inline constexpr float PlayerAttackCooldown = 0.45f;
inline constexpr float WeaponSpawnOffsetX = 60.f;
inline constexpr float WeaponSpawnOffsetY = -8.f;
inline constexpr float SecondWeaponSpawnMapX = 1700.f;
inline constexpr float SecondWeaponSpawnMapY = 520.f;
inline constexpr float WeaponPickupRadius = 80.f;
inline constexpr float EquippedWeaponFacingOffset = 18.f;
inline constexpr float EquippedWeaponHeightOffset = -58.f;

inline constexpr float WalkableMinimumMapY = 400.f;
inline constexpr float WalkableMaximumMapY = 680.f;
inline constexpr float EnemyMinimumSpawnMapY = 510.f;
inline constexpr float EnemyMaximumSpawnMapY = 550.f;
inline constexpr float EnemyEntryOffsetMapX = 120.f;
inline constexpr float EnemyEntryArrivalDistance = 8.f;
inline constexpr ArenaLayout FirstArena{300.f, 1650.f, 450.f, 900.f, 5};
inline constexpr ArenaLayout SecondArena{1800.f, 2950.f, 2270.f, 2450.f, 5};
inline constexpr ArenaLayout ThirdArena{3100.f, 4250.f, 3150.f, 3550.f, 5};

inline constexpr CollisionObstacle BuildingFrontBoundary{
    {0.f, 460.f, MapImageWidth, 4.f},
    true,
    false
};
inline constexpr std::array CollisionObstacles{BuildingFrontBoundary};

inline constexpr float BodyHalfWidth = 28.f;
inline constexpr float BodyHeight = 36.f;
inline constexpr float PlayerBulletSpawnOffsetX = 38.f;
inline constexpr float PlayerBulletSpawnOffsetY = -74.f;
inline constexpr float PlayerBulletSpeed = 700.f;
inline const sf::Color PlayerBulletColor(250, 210, 80);

inline constexpr float HealthBarWidth = 220.f;
inline constexpr float HealthBarHeight = 18.f;
inline constexpr float HealthBarMargin = 20.f;
inline constexpr float EnemyHealthBarRightOffset = 240.f;
inline const sf::Color HealthBarBackground(30, 30, 30, 220);
inline const sf::Color PlayerHealthBarColor(55, 190, 85);
inline const sf::Color EnemyHealthBarColor(200, 55, 45);

inline constexpr float AmmoPanelX = 20.f;
inline constexpr float AmmoPanelY = 46.f;
inline constexpr float AmmoPanelWidth = 108.f;
inline constexpr float AmmoPanelHeight = 40.f;
inline const sf::Color AmmoPanelColor(25, 25, 25, 220);
inline const sf::Color AmmoIconColor(235, 220, 165);
inline const sf::Color AmmoDigitColor(245, 220, 110);
inline constexpr float AmmoIconBodyX = 32.f;
inline constexpr float AmmoIconBodyY = 63.f;
inline constexpr float AmmoIconBodyWidth = 13.f;
inline constexpr float AmmoIconBodyHeight = 5.f;
inline constexpr float AmmoIconTipRadius = 2.5f;
inline constexpr float AmmoIconTipX = 46.f;
inline constexpr float AmmoIconTipY = 65.5f;
inline constexpr float AmmoFirstDigitX = 57.f;
inline constexpr float AmmoSecondDigitX = 78.f;
inline constexpr float AmmoDigitY = 52.f;
inline constexpr Rect AmmoSegmentTop{4.f, 0.f, 11.f, 3.f};
inline constexpr Rect AmmoSegmentUpperRight{14.f, 3.f, 3.f, 10.f};
inline constexpr Rect AmmoSegmentLowerRight{14.f, 14.f, 3.f, 10.f};
inline constexpr Rect AmmoSegmentBottom{4.f, 24.f, 11.f, 3.f};
inline constexpr Rect AmmoSegmentLowerLeft{0.f, 14.f, 3.f, 10.f};
inline constexpr Rect AmmoSegmentUpperLeft{0.f, 3.f, 3.f, 10.f};
inline constexpr Rect AmmoSegmentMiddle{4.f, 12.f, 11.f, 3.f};
}


namespace Level2
{
    inline constexpr const char* BackgroundPath = "assets/level_pandemia/fondo_pandemia_dia.png";
    inline constexpr float MapImageWidth = 3840.f;
    inline constexpr float MapImageHeight = 1080.f;

    inline constexpr float FirstWaveTriggerX = 900.f;
    inline constexpr float FirstWaveMinX = 700.f;
    inline constexpr float FirstWaveMaxX = 1800.f;
    inline constexpr float FirstWaveSpawnX = 1400.f;
    inline constexpr float FirstWaveSpawnY = 900.f;

    inline constexpr float PlayerStartMapX = 300.f;

    inline constexpr float PlayerStartMapY = 900.f;
    inline constexpr float PlayerVisualScale = 2.5f;
    inline constexpr char PlayerWalkSpritePath[] =
        "assets/level_pandemia/caminar.png";
    inline constexpr char PlayerMaskedWalkSpritePath[] =
        "assets/level_pandemia/caminar_tapabocas.png";
    inline constexpr char PlayerAttackSpritePath[] =
        "assets/level_pandemia/ataque_golpes.png";
    inline constexpr char PlayerDeathSpritePath[] =
        "assets/level_pandemia/morir.png";
    inline constexpr float TapabocasMapX = 1038.f;
    inline constexpr float TapabocasMapY = 866.f;
    inline constexpr float TapabocasEquipRadius = 90.f;
    inline constexpr char SerpentAttackSpritePath[] =
        "assets/level_pandemia/serpiente_murcielago_ataque.png";
    inline constexpr float SerpentMapX = MapImageWidth - 200.f;
    inline constexpr float SerpentMapY = 900.f;
    inline constexpr float SerpentVisualScale = 2.5f;
    inline constexpr float SerpentDetectionRange = 700.f;
    inline constexpr float SerpentAttackRange = 110.f;
    inline constexpr int SerpentAttackDamage = 20;
    inline constexpr float SerpentAttackCooldown = 1.f;
    inline constexpr float MaskDuration = 5.f;
    inline constexpr int PlayerAttackDamage = 20;
    inline constexpr float PlayerAttackRange = 80.f;
    inline constexpr float PlayerAttackCooldown = 0.35f;

    inline constexpr float Cientifico1StartMapX = 1600.f;
    inline constexpr float Cientifico1StartMapY = 900.f;
    inline constexpr float Cientifico1VisualScale = 1.f;
    inline constexpr float Cientifico1InteractRadius = 110.f;
    inline constexpr float Cientifico1FollowDistance = 95.f;

    inline constexpr float EnemyEntryOffsetMapX = 250.f;
    inline constexpr float EnemyEntryArrivalDistance = 12.f;
    inline constexpr float EnemyVisualScale = 1.f;
    inline constexpr float BodyHalfWidth = 24.f;
    inline constexpr float BodyHeight = 64.f;

    inline constexpr float EquippedWeaponFacingOffset = 28.f;
    inline constexpr float EquippedWeaponHeightOffset = -24.f;

    inline constexpr float HealthBarMargin = 20.f;
    inline constexpr float HealthBarWidth = 220.f;
    inline constexpr float HealthBarHeight = 18.f;
    inline constexpr float EnemyHealthBarRightOffset = 240.f;
    inline const sf::Color HealthBarBackground{40, 40, 40, 220};
    inline const sf::Color PlayerHealthBarColor{70, 210, 110, 255};
    inline const sf::Color EnemyHealthBarColor{220, 65, 65, 255};

    inline constexpr float PlayerBulletSpawnOffsetX = 36.f;
    inline constexpr float PlayerBulletSpawnOffsetY = -24.f;
    inline constexpr float PlayerBulletSpeed = 950.f;
    inline const sf::Color PlayerBulletColor{255, 220, 90, 255};

    inline constexpr CollisionObstacle Obstacles[] = {
        {{800.f, 760.f, 120.f, 120.f}, true, true}
    };
}

namespace Robot
{
inline constexpr char DataPath[] = "data/robot.json";
inline constexpr float DetectionRange = 1800.f;
inline constexpr float MeleeRange = 95.f;
inline constexpr float MeleeChaseRange = 210.f;
inline constexpr int MeleeDamage = 18;
inline constexpr float MeleeCooldown = 1.0f;
inline constexpr float LaserRange = 620.f;
inline constexpr float LaserCooldown = 1.35f;
inline constexpr float LaserSpeed = 760.f;
inline constexpr int LaserDamage = 14;
inline constexpr float LaneTolerance = 14.f;
inline constexpr float MuzzleOffset = 54.f;
inline constexpr float MuzzleHeight = 64.f;
inline constexpr int ExperienceReward = 65;
inline const sf::Color LaserColor(60, 230, 255);
}

namespace FutureLevel1
{
struct ArenaLayout
{
    float minimumMapX;
    float maximumMapX;
    float spawnMinimumMapX;
    float spawnMaximumMapX;
    std::size_t robotCount;
};

inline constexpr char BackgroundPath[] = "assets/futuro/fondo_fut.jpeg";
inline constexpr float MapImageWidth = 5856.f;
inline constexpr float MapImageHeight = 704.f;

inline constexpr float PlayerStartMapX = 200.f;
inline constexpr float PlayerStartMapY = 615.f;
inline constexpr float PlayerVisualScale = 2.5f;
inline constexpr float EnemyVisualScale = 2.5f;
inline constexpr int PlayerAttackDamage = 35;
inline constexpr float PlayerAttackRange = 130.f;
inline constexpr float PlayerAttackCooldown = 0.45f;
inline constexpr float WeaponSpawnOffsetX = 70.f;
inline constexpr float WeaponSpawnOffsetY = -8.f;
inline constexpr float WeaponPickupRadius = 80.f;
inline constexpr float EquippedWeaponFacingOffset = 18.f;
inline constexpr float EquippedWeaponHeightOffset = -58.f;

// Franja caminable de la calle (sin obstáculos internos)
inline constexpr float WalkableMinimumMapY = 545.f;
inline constexpr float WalkableMaximumMapY = 680.f;
inline constexpr float EnemyMinimumSpawnMapY = 585.f;
inline constexpr float EnemyMaximumSpawnMapY = 640.f;
inline constexpr float EnemyEntryOffsetMapX = 120.f;
inline constexpr float EnemyEntryArrivalDistance = 8.f;

// Fogata animada y daño por quemadura
inline constexpr float CampfireMapX = 4665.f;
inline constexpr float CampfireMapY = 600.f;
inline constexpr float CampfireBurnRadiusMap = 78.f;
inline constexpr int CampfireBurnDamage = 8;
inline constexpr float CampfireBurnInterval = 0.75f;

// Drops de enemigos (botiquines y armas)
inline constexpr int MedkitDropChancePercent = 35;
inline constexpr int WeaponDropChancePercent = 15;
inline constexpr int MedkitHealAmount = 45;
inline constexpr float MedkitPickupRadius = 52.f;

inline constexpr ArenaLayout FirstArena{350.f, 1950.f, 950.f, 1650.f, 3};
inline constexpr ArenaLayout SecondArena{2150.f, 3850.f, 2800.f, 3550.f, 4};
inline constexpr ArenaLayout ThirdArena{4050.f, 5750.f, 4700.f, 5450.f, 5};

inline constexpr float BodyHalfWidth = 28.f;
inline constexpr float PlayerBulletSpawnOffsetX = 38.f;
inline constexpr float PlayerBulletSpawnOffsetY = -74.f;
inline constexpr float PlayerBulletSpeed = 700.f;
inline const sf::Color PlayerBulletColor(250, 210, 80);
}
namespace LevelState
{
inline constexpr float CameraFollowSpeed = 5.f;
}

namespace Menu
{
struct ButtonLayout
{
    float x;
    float y;
    float width;
    float height;
};

inline constexpr int ReferenceWidth = 1920;
inline constexpr int ReferenceHeight = 1080;
inline constexpr std::size_t ButtonCount = 3;
inline constexpr int FrameWidth = 1280;
inline constexpr int FrameCount = 4;
inline constexpr float FrameDuration = 0.22f;
inline constexpr char BackgroundPath[] = "assets/fondo_animado.png";
inline constexpr float CloseButtonRadius = 20.f;
inline constexpr float CloseButtonRightOffset = 55.f;
inline constexpr float CloseButtonTopOffset = 15.f;
inline constexpr float CloseBoundsSize = 40.f;
inline constexpr float CloseCrossSize = 20.f;
inline constexpr float CloseCrossThickness = 3.f;
inline constexpr float CloseCrossOriginX = 10.f;
inline constexpr float CloseCrossOriginY = 1.5f;
inline constexpr float CloseCrossOffset = 20.f;
inline constexpr float CloseCrossAngle = 45.f;
inline constexpr float ButtonOutlineThickness = 2.f;
inline constexpr ButtonLayout NewGameButton{686.f, 509.f, 560.f, 85.f};
inline constexpr ButtonLayout ContinueButton{686.f, 643.f, 560.f, 83.f};
inline constexpr ButtonLayout ExitButton{686.f, 772.f, 560.f, 92.f};
inline const sf::Color CloseButtonColor(100, 35, 35, 220);
inline const sf::Color CloseButtonHoverColor(200, 55, 55, 240);
}
}
