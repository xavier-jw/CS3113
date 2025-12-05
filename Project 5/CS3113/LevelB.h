#pragma once
#include "Scene.h"

class LevelB : public Scene
{
public:
    // Constructor
    LevelB(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    // Texture for Player
    Texture2D mTexPlayerWalk;
    Texture2D mTexPlayerAttack;
    Texture2D mTexPlayerDeath;

    Texture2D mTexTree1;
    Texture2D mTexTree2;
    Texture2D mTexTree3;
    Texture2D mTexTree4;

    // Texture for Slime
    Texture2D mTexSlimeWalk;
    Texture2D mTexSlimeAttack;
    Texture2D mTexSlimeDeath;

    std::map<Direction, std::vector<int>> mWalkAnim;
    std::map<Direction, std::vector<int>> mAttackAnim;
    std::map<Direction, std::vector<int>> mDeathAnim;

    std::map<Direction, std::vector<int>> mSlimeWalkAnim;
    std::map<Direction, std::vector<int>> mSlimeAttackAnim;
    std::map<Direction, std::vector<int>> mSlimeDeathAnim;
};