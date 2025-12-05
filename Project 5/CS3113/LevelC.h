#pragma once
#include "Scene.h"

class LevelC : public Scene
{
public:
    // Constructor
    LevelC(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    Texture2D mTexPlayerWalk;
    Texture2D mTexPlayerAttack;
    Texture2D mTexPlayerDeath;

    Texture2D mTexTree1;
    Texture2D mTexTree2;
    Texture2D mTexTree3;
    Texture2D mTexTree4;

    // Texture for Demon
    Texture2D mTexDemonWalk;
    Texture2D mTexDemonAttack;
    Texture2D mTexDemonDeath;

    // Player animation maps
    std::map<Direction, std::vector<int>> mWalkAnim;
    std::map<Direction, std::vector<int>> mAttackAnim;
    std::map<Direction, std::vector<int>> mDeathAnim;

    // Demon animation maps
    std::map<Direction, std::vector<int>> mDemonWalkAnim;
    std::map<Direction, std::vector<int>> mDemonAttackAnim;
    std::map<Direction, std::vector<int>> mDemonDeathAnim;
};