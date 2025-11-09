#pragma once
#include "Scene.h"

// Level A, inherite from Scene
class LevelA : public Scene
{
public:
    LevelA(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}
    ~LevelA() override = default;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};