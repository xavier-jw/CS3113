#pragma once
#include "Entity.h"
#include "Map.h"

// Scene ID enumeration, used for scene transitions
enum SceneID
{
    SCENE_MENU,
    SCENE_LEVEL_A,
    SCENE_LEVEL_B,
    SCENE_LEVEL_C,
    SCENE_WIN,
    SCENE_LOSE
};

struct GameState
{
    Entity *player = nullptr;
    Map *map = nullptr;
    Entity *enemy = nullptr;

    Music bgm;
    Sound jumpSound;
    Sound bumpSound;

    Camera2D camera = {0}; // Scene camera

    int nextSceneID = -1; // ID for the next scene (-1 = no switch)
};

// Scene Base Class
class Scene
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";

public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);
    virtual ~Scene() {} // virtual destructor

    // Pure virtual function
    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    GameState &getState() { return mGameState; }
};