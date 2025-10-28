#ifndef ENTITY_H
#define ENTITY_H

#include "cs3113.h"

enum Direction
{
    LEFT,
    UP,
    RIGHT,
    DOWN
};

enum EntityStatus
{
    ACTIVE,
    INACTIVE
};

class Entity
{
private:
    // Physical Attributes
    Vector2 mPosition;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions; // Collision box dimensions

    Texture2D mTexture;       // Texture
    TextureType mTextureType; // Texture Type (SINGLE or ATLAS)
    float mAngle;             // Rotation angle

    // Animation Attributes
    Vector2 mSpriteSheetDimensions;
    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int> mAnimationIndices;
    Direction mDirection;
    int mFrameSpeed;
    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;

    EntityStatus mEntityStatus = ACTIVE;

    // Private animate function
    void animate(float deltaTime);

public:
    // Public Constants
    static constexpr int DEFAULT_SIZE = 250;
    static constexpr int DEFAULT_FRAME_SPEED = 14;

    // Constructor and destructor function
    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
           TextureType textureType, Vector2 spriteSheetDimensions,
           std::map<Direction, std::vector<int>> animationAtlas);
    ~Entity();

    // Core methods
    void update(float deltaTime);
    void render();
    bool isColliding(Entity *other) const;

    // Helper Function
    void activate() { mEntityStatus = ACTIVE; }
    void deactivate() { mEntityStatus = INACTIVE; }
    bool isActive() { return mEntityStatus == ACTIVE ? true : false; }
    void displayCollider();

    // -------- Getters -------- //
    Vector2 getPosition() const { return mPosition; }
    Vector2 getVelocity() const { return mVelocity; }
    Vector2 getScale() const { return mScale; }
    Vector2 getColliderDimensions() const { return mColliderDimensions; }

    // -------- Setters -------- //
    void setPosition(Vector2 newPosition)
    {
        mPosition = newPosition;
    }
    void setVelocity(Vector2 newVelocity)
    {
        mVelocity = newVelocity;
    }
    void setAcceleration(Vector2 newAcceleration)
    {
        mAcceleration = newAcceleration;
    }
    void addAcceleration(Vector2 deltaAcceleration)
    {
        mAcceleration = Vector2Add(mAcceleration, deltaAcceleration);
    }
    void setDirection(Direction d) { mDirection = d; }
    void setColliderDimensions(Vector2 newDimensions)
    {
        mColliderDimensions = newDimensions;
    }
    void setFrameSpeed(int newSpeed)
    {
        mFrameSpeed = newSpeed;
    }
};

#endif // ENTITY_H