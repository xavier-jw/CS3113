#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"

// Enumeration: Direction (for animation and movement)
enum Direction
{
    LEFT,
    UP,
    RIGHT,
    DOWN
};

// Enumeration: Entity State
enum EntityStatus
{
    ACTIVE,
    INACTIVE
};

// Enumeration: Entity Type
enum EntityType
{
    PLAYER,
    BLOCK,
    PLATFORM,
    NPC,
    NONE
};

// Enumeration: AI Type
enum AIType
{
    WANDERER,
    FOLLOWER,
    FLYER
};

// Enumeration: AI State (for state machine)
enum AIState
{
    WALKING,
    IDLE,
    FOLLOWING,
    FLYING
};

class Entity
{
private:
    // Physical Properties
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions; // Collision box scale

    Texture2D mTexture;
    TextureType mTextureType; // Texture type (SINGLE or ATLAS)
    Vector2 mSpriteSheetDimensions;

    // For Animation
    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int> mAnimationIndices;
    Direction mDirection;
    int mFrameSpeed;
    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;

    // Jump Status
    bool mIsJumping = false;
    float mJumpingPower = 0.0f;

    int mSpeed;
    float mAngle;

    // Collision signal
    bool mIsCollidingTop = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight = false;
    bool mIsCollidingLeft = false;

    EntityStatus mEntityStatus = ACTIVE; // Entity state (whether activated)
    EntityType mEntityType;              // Entity type (player, ai, ...)

    // AI
    AIType mAIType;
    AIState mAIState;

    // Private Methods
    /**
     * @brief Check for collision between the Y-axis and the map
     */
    void checkCollisionY(Map *map);

    /**
     * @brief Check for collision between the X-axis and the map
     */
    void checkCollisionX(Map *map);

    /**
     * @brief Reset all collision flags (called at the start of the update)
     */
    void resetColliderFlags()
    {
        mIsCollidingTop = false;
        mIsCollidingBottom = false;
        mIsCollidingRight = false;
        mIsCollidingLeft = false;
    }

    void animate(float deltaTime);
    void AIActivate(Entity *target);
    void AIWander();
    void AIFollow(Entity *target);
    void AIFlyer();

public:
    // Static Constants
    static constexpr int DEFAULT_SIZE = 250;
    static constexpr int DEFAULT_SPEED = 200;
    static constexpr int DEFAULT_FRAME_SPEED = 14;
    // Collision threshold
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    // Constructor/Destructor
    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
           EntityType entityType);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
           TextureType textureType, Vector2 spriteSheetDimensions,
           std::map<Direction, std::vector<int>> animationAtlas,
           EntityType entityType);
    ~Entity();

    // Core Methods
    void update(float deltaTime, Entity *player, Map *map,
                Entity *collidableEntities, int collisionCheckCount);
    void render();
    void normaliseMovement() { Normalise(&mMovement); }

    // Action
    void jump() { mIsJumping = true; }
    void activate() { mEntityStatus = ACTIVE; }
    void deactivate() { mEntityStatus = INACTIVE; }
    void displayCollider(); // Debugging: Display collision box

    bool isActive() { return mEntityStatus == ACTIVE ? true : false; }

    // Movement helper functions
    void moveUp()
    {
        mMovement.y = -1;
        mDirection = UP;
    }
    void moveDown()
    {
        mMovement.y = 1;
        mDirection = DOWN;
    }
    void moveLeft()
    {
        mMovement.x = -1;
        mDirection = LEFT;
    }
    void moveRight()
    {
        mMovement.x = 1;
        mDirection = RIGHT;
    }
    void resetMovement() { mMovement = {0.0f, 0.0f}; }

    // -------- Getters ---------
    Vector2 getPosition() const { return mPosition; }
    Vector2 getMovement() const { return mMovement; }
    Vector2 getVelocity() const { return mVelocity; }
    Vector2 getAcceleration() const { return mAcceleration; }
    Vector2 getScale() const { return mScale; }
    Vector2 getColliderDimensions() const { return mColliderDimensions; }
    Vector2 getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D getTexture() const { return mTexture; }
    TextureType getTextureType() const { return mTextureType; }
    Direction getDirection() const { return mDirection; }
    int getFrameSpeed() const { return mFrameSpeed; }
    float getJumpingPower() const { return mJumpingPower; }
    bool isJumping() const { return mIsJumping; }
    int getSpeed() const { return mSpeed; }
    float getAngle() const { return mAngle; }
    EntityType getEntityType() const { return mEntityType; }
    AIType getAIType() const { return mAIType; }
    AIState getAIState() const { return mAIState; }

    // Collision state Getters
    bool isCollidingTop() const { return mIsCollidingTop; }
    bool isCollidingBottom() const { return mIsCollidingBottom; }

    /**
     * @brief Check whether this entity collides with another entity (AABB)
     */
    bool isColliding(Entity *other) const;

    std::map<Direction, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }

    // ------- Setters ------
    void setPosition(Vector2 newPosition)
    {
        mPosition = newPosition;
    }
    void setMovement(Vector2 newMovement)
    {
        mMovement = newMovement;
    }
    void setAcceleration(Vector2 newAcceleration)
    {
        mAcceleration = newAcceleration;
    }
    void setScale(Vector2 newScale)
    {
        mScale = newScale;
    }
    void setTexture(const char *textureFilepath)
    {
        mTexture = LoadTexture(textureFilepath);
    }
    void setColliderDimensions(Vector2 newDimensions)
    {
        mColliderDimensions = newDimensions;
    }
    void setSpriteSheetDimensions(Vector2 newDimensions)
    {
        mSpriteSheetDimensions = newDimensions;
    }
    void setSpeed(int newSpeed)
    {
        mSpeed = newSpeed;
    }
    void setFrameSpeed(int newSpeed)
    {
        mFrameSpeed = newSpeed;
    }
    void setJumpingPower(float newJumpingPower)
    {
        mJumpingPower = newJumpingPower;
    }
    void setAngle(float newAngle)
    {
        mAngle = newAngle;
    }
    void setEntityType(EntityType entityType)
    {
        mEntityType = entityType;
    }
    void setDirection(Direction newDirection)
    {
        mDirection = newDirection;
        if (mTextureType == ATLAS)
            mAnimationIndices = mAnimationAtlas.at(mDirection);
    }
    void setAIState(AIState newState)
    {
        mAIState = newState;
    }
    void setAIType(AIType newType)
    {
        mAIType = newType;
    }
};

#endif // ENTITY_H