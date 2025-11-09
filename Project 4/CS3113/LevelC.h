#pragma once
#include "Scene.h"

// Level C, inherite from Scene
class LevelC : public Scene
{
public:
    LevelC(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}
    ~LevelC() override = default;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};