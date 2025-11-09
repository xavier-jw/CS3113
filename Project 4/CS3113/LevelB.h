#pragma once
#include "Scene.h"

// Level B, inherite from Scene
class LevelB : public Scene
{
public:
    LevelB(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}
    ~LevelB() override = default;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};