#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <array.hpp>
#include "GameConfig.hpp"
#include "json.hpp"

struct AnimationClip {
    std::string name = "";
    int row = 0;
    int frameCount = 1;
    float frameDuration = GameConfig::Animation::DefaultFrameDuration;
    bool loop = true;
};

class Animation {
private:
    int frameWidth = GameConfig::Animation::DefaultFrameWidth;
    int frameHeight = GameConfig::Animation::DefaultFrameHeight;
    sf::Sprite* targetSprite = nullptr;
    Designar::DynArray<AnimationClip> clips;

    int currentClipIndex = -1;
    std::string currentClipName = "";
    float timer = 0.f;
    int currentFrame = 0;
    bool finished = false;
    bool facingRight = true;

    int findClipIndex(const std::string& name) const;
    void applyTextureRect();

public:
    Animation(
        int width = GameConfig::Animation::DefaultFrameWidth,
        int height = GameConfig::Animation::DefaultFrameHeight);

    void attachSprite(sf::Sprite& sprite);
    void setFrameSize(int width, int height);
    void addClip(
        const std::string& name,
        int row,
        int frames,
        float duration = GameConfig::Animation::DefaultFrameDuration,
        bool loop = true);
    void loadFromJson(const nlohmann::json& animJson);
    void play(const std::string& name, bool forceRestart = false);
    void setCurrentFrame(int frame);
    void setFacingRight(bool right);
    void update(float dt);

    bool isFinished() const;
    const std::string& getCurrentClip() const;
};