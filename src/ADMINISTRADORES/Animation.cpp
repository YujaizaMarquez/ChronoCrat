#include "Animation.hpp"
#include <cmath>

Animation::Animation(int width, int height)
    : frameWidth(width), frameHeight(height) {}

int Animation::findClipIndex(const std::string& name) const {
    for (size_t i = 0; i < clips.size(); ++i) {
        if (clips[i].name == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void Animation::applyTextureRect() {
    if (!targetSprite || currentClipIndex < 0) return;
    const auto& clip = clips[currentClipIndex];
    int left = currentFrame * frameWidth;
    int top  = clip.row * frameHeight;

    targetSprite->setTextureRect(sf::IntRect({left, top}, {frameWidth, frameHeight}));
}

void Animation::attachSprite(sf::Sprite& sprite) {
    targetSprite = &sprite;
    targetSprite->setOrigin({frameWidth / 2.f, static_cast<float>(frameHeight)});
    targetSprite->setTextureRect(sf::IntRect({0, 0}, {frameWidth, frameHeight}));
}

void Animation::addClip(const std::string& name, int row, int frames, float duration, bool loop) {
    int idx = findClipIndex(name);
    if (idx != -1) {
        clips[idx] = {name, row, frames, duration, loop};
    } else {
        clips.append({name, row, frames, duration, loop});
    }
}

void Animation::loadFromJson(const nlohmann::json& animJson) {
    for (auto it = animJson.begin(); it != animJson.end(); ++it) {
        const auto& val = it.value();
        addClip(
            it.key(),
            val.value("row", 0),
            val.value("frames", 1),
            val.value("duration", 0.12f),
            val.value("loop", true)
        );
    }
}

void Animation::play(const std::string& name, bool forceRestart) {
    int idx = findClipIndex(name);
    if (idx == -1) return;
    if (currentClipIndex == idx && !forceRestart) return;

    currentClipIndex = idx;
    currentClipName = name;
    currentFrame = 0;
    timer = 0.f;
    finished = false;
    applyTextureRect();
}

void Animation::setFacingRight(bool right) {
    facingRight = right;
    if (!targetSprite) return;
    sf::Vector2f scale = targetSprite->getScale();
    float absX = std::abs(scale.x);
    targetSprite->setScale({facingRight ? absX : -absX, scale.y});
}

void Animation::update(float dt) {
    if (!targetSprite || currentClipIndex < 0 || finished) return;

    const auto& clip = clips[currentClipIndex];
    timer += dt;

    if (timer >= clip.frameDuration) {
        timer -= clip.frameDuration;
        if (currentFrame + 1 < clip.frameCount) {
            currentFrame++;
        } else {
            if (clip.loop) {
                currentFrame = 0;
            } else {
                finished = true;
            }
        }
        applyTextureRect();
    }
}

bool Animation::isFinished() const {
    return finished;
}

const std::string& Animation::getCurrentClip() const {
    return currentClipName;
}