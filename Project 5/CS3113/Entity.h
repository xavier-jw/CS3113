#pragma once
#include "Map.h"
#include "cs3113.h"
#include <vector>
#include <map>

// Define character direction
enum Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Entity type (Player, Enemy, Static Prop)
enum EntityType
{
    PLAYER,
    ENEMY,
    PROP
};

// Enemy AI behavior type
enum AIType
{
    SOLDIER, // Simple patrol and chase
    SLIME,   // Circular patrol, provoked on hit
    DEMON    // Boss AI with attack cooldown
};

// AI states
enum AIState
{
    IDLE,
    WALKING,
    ATTACKING,
    DYING,
    DEAD
};

enum EntityStatus
{
    ACTIVE,
    INACTIVE
};

class Entity
{
private:
    EntityStatus mEntityStatus = ACTIVE;
    EntityType mEntityType;
    AIType mAIType;
    AIState mAIState;

    // Soldier AI Logic
    bool mIsProvoked = false;

    // Slime AI Logic
    Vector2 mPatrolCenter = {0.0f, 0.0f};
    float mPatrolRadius = 60.0f; // Radius of patrol circle
    float mPatrolAngle = 0.0f;

    // Cooldown timer after attacking
    float mAttackCooldown = 0.0f;

    Vector2 mPosition;
    Vector2 mMovement; // Input vector (-1 to 1)
    Vector2 mScale;
    Vector2 mVelocity;
    float mSpeed;

    Vector2 mColliderDimensions; // Collider box dimensions

    int mHealth = 6;
    int mAttackPower = 1;

    // --- Animation & Texture ---
    Texture2D mTexture;
    TextureType mTextureType;
    float mAngle = 0.0f;
    std::map<Direction, std::vector<int>> mAnimationAtlas; // Animation atlas mapping
    std::vector<int> mAnimationIndices;                    // Frame sequence for current action
    int mFrameSpeed;
    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;
    Vector2 mSpriteSheetDimensions; // Sprite sheet rows/cols
    Direction mDirection = DOWN;

    // Has damage been dealt in current attack animation
    bool mHasDealtDamage = false;

public:
    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction, std::vector<int>> animationAtlas, EntityType entityType);
    virtual ~Entity();

    void update(float deltaTime, Entity *player, Map *map, std::vector<Entity *> *obstacles = nullptr);
    void render();

    void displayCollider();

    void checkCollisionY(Map *map);
    void checkCollisionX(Map *map);

    // Entity-vs-Entity collision
    void checkCollisionWithObstacles(std::vector<Entity *> *obstacles);
    bool isColliding(Entity *other) const;

    // --- Getters & Setters ---
    void setPosition(Vector2 position) { mPosition = position; }
    Vector2 getPosition() const { return mPosition; }
    void setMovement(Vector2 movement) { mMovement = movement; }
    Vector2 getMovement() const { return mMovement; }
    void normaliseMovement();
    void setVelocity(Vector2 velocity) { mVelocity = velocity; }
    Vector2 getVelocity() const { return mVelocity; }
    void setSpeed(float speed) { mSpeed = speed; }
    float getSpeed() const { return mSpeed; }
    void setColliderDimensions(Vector2 dim) { mColliderDimensions = dim; }
    Vector2 getColliderDimensions() const { return mColliderDimensions; }
    void setAIType(AIType type) { mAIType = type; }
    void setAIState(AIState state) { mAIState = state; }
    AIState getAIState() const { return mAIState; }
    void setHealth(int health) { mHealth = health; }
    int getHealth() const { return mHealth; }
    void setAttackPower(int power) { mAttackPower = power; }
    int getAttackPower() const { return mAttackPower; }

    // Switch texture (Used to switch atlases for different states)
    void setTexture(Texture2D newTexture, int cols, int rows, std::map<Direction, std::vector<int>> newAtlas);

    void setPatrolCenter(Vector2 center) { mPatrolCenter = center; }

    Rectangle getAttackBox() const;
    int getCurrentFrameIndex() const { return mCurrentFrameIndex; }
    void setHasDealtDamage(bool value) { mHasDealtDamage = value; }
    bool hasDealtDamage() const { return mHasDealtDamage; }

    //  Movement helper functions (Prevent movement while attacking)
    void moveUp()
    {
        if (mAIState != ATTACKING)
        {
            mMovement.y = -1.0f;
            mDirection = UP;
        }
    }
    void moveDown()
    {
        if (mAIState != ATTACKING)
        {
            mMovement.y = 1.0f;
            mDirection = DOWN;
        }
    }
    void moveLeft()
    {
        if (mAIState != ATTACKING)
        {
            mMovement.x = -1.0f;
            mDirection = LEFT;
        }
    }
    void moveRight()
    {
        if (mAIState != ATTACKING)
        {
            mMovement.x = 1.0f;
            mDirection = RIGHT;
        }
    }
    void resetMovement() { mMovement = {0.0f, 0.0f}; }

    void aiActivate(Entity *player, float deltaTime);
    void aiSoldier(Entity *player);
    void aiSlime(Entity *player, float deltaTime);

    void aiDemon(Entity *player, float deltaTime);

    void aiPatrol();
    void aiChase(Entity *player);

    void takeDamage(int damage);
    void animate(float deltaTime);

    void unloadTexture();
};