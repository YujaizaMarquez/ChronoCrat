#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <array.hpp>
#include "json.hpp"

struct AnimationClip {
    std::string name = "";
    int row = 0;
    int frameCount = 1;
    float frameDuration = 0.12f;
    bool loop = true;
};

class Animation {
private:
    int frameWidth = 64;
    int frameHeight = 64;
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
    Animation(int width = 64, int height = 64);

    void attachSprite(sf::Sprite& sprite);
    void addClip(const std::string& name, int row, int frames, float duration = 0.12f, bool loop = true);
    void loadFromJson(const nlohmann::json& animJson);
    void play(const std::string& name, bool forceRestart = false);
    void setFacingRight(bool right);
    void update(float dt);

    bool isFinished() const;
    const std::string& getCurrentClip() const;
};