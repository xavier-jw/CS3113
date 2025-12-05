#pragma once
#include "Scene.h"

// Level A class, inheriting from the base Scene class
class LevelA : public Scene
{
private:
    // Textures for Player
    Texture2D mTexPlayerWalk, mTexPlayerAttack, mTexPlayerDeath;
    // Textures for Soldier Enemy
    Texture2D mTexSoldierWalk, mTexSoldierAttack, mTexSoldierDeath;
    // Texture for Windmill
    Texture2D mTexWindmill;
    // Animation atlas maps (Direction -> List of frame indices)
    std::map<Direction, std::vector<int>> mWalkAnim, mAttackAnim, mDeathAnim;
    std::map<Direction, std::vector<int>> mWindmillAnim;

public:
    LevelA(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}
    ~LevelA() override = default;
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};