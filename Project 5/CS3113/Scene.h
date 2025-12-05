#pragma once
#include "Entity.h"
#include "Map.h"
#include "cs3113.h"
#include <vector>

enum SceneID
{
    SCENE_MENU,
    SCENE_LEVEL_A,
    SCENE_LEVEL_B,
    SCENE_LEVEL_C,
    SCENE_LOSE,
    SCENE_WIN
};

struct GameState
{
    Entity *player = nullptr;
    Map *map = nullptr;
    std::vector<Entity *> enemies;
    std::vector<Entity *> scenery;

    Music bgm;
    Sound attackSound;
    Sound hurtSound;

    Camera2D camera = {0};
    int nextSceneID = -1;
};

class Scene
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";

public:
    Scene() : mOrigin{0.0f, 0.0f} {}
    Scene(Vector2 origin, const char *bgHexCode) : mOrigin{origin}, mBGColourHexCode{bgHexCode} {}
    virtual ~Scene() {}

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    GameState &getState() { return mGameState; }
};