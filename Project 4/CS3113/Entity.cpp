#include "Entity.h"

// Constructor functions
Entity::Entity() : mPosition{0.0f, 0.0f}, mMovement{0.0f, 0.0f},
                   mVelocity{0.0f, 0.0f}, mAcceleration{0.0f, 0.0f},
                   mScale{DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions{DEFAULT_SIZE, DEFAULT_SIZE},
                   mTexture{}, mTextureType{SINGLE}, mAngle{0.0f},
                   mSpriteSheetDimensions{}, mDirection{RIGHT},
                   mAnimationAtlas{{}}, mAnimationIndices{}, mFrameSpeed{0},
                   mEntityType(NONE), mAIType(WANDERER), mAIState(IDLE) {}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType) : mPosition{position}, mVelocity{0.0f, 0.0f}, mAcceleration{0.0f, 0.0f},
                                                                                                      mScale{scale}, mMovement{0.0f, 0.0f}, mColliderDimensions{scale},
                                                                                                      mTexture{LoadTexture(textureFilepath)}, mTextureType{SINGLE},
                                                                                                      mDirection{RIGHT}, mAnimationAtlas{{}}, mAnimationIndices{},
                                                                                                      mFrameSpeed{0}, mSpeed{DEFAULT_SPEED}, mAngle{0.0f},
                                                                                                      mEntityType(entityType), mAIType(WANDERER), mAIState(IDLE) {}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
               TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction, std::vector<int>> animationAtlas, EntityType entityType) : mPosition{position},
                                                                                                                                                      mVelocity{0.0f, 0.0f}, mAcceleration{0.0f, 0.0f},
                                                                                                                                                      mMovement{0.0f, 0.0f}, mScale{scale}, mColliderDimensions{scale},
                                                                                                                                                      mTexture{LoadTexture(textureFilepath)}, mTextureType{ATLAS},
                                                                                                                                                      mSpriteSheetDimensions{spriteSheetDimensions},
                                                                                                                                                      mAnimationAtlas{animationAtlas}, mDirection{RIGHT},
                                                                                                                                                      mAnimationIndices{animationAtlas.at(RIGHT)},
                                                                                                                                                      mFrameSpeed{DEFAULT_FRAME_SPEED}, mAngle{0.0f},
                                                                                                                                                      mSpeed{DEFAULT_SPEED},
                                                                                                                                                      mEntityType(entityType), mAIType(WANDERER), mAIState(IDLE) {}
// Destructor
Entity::~Entity() { UnloadTexture(mTexture); };

// ---------- AI Logic -----------

/**
 * @brief AI Activation
 */
void Entity::AIActivate(Entity *player)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;
    case FOLLOWER:
        AIFollow(player);
        break;
    case FLYER:
        AIFlyer();
        break;
    default:
        break;
    }
}

/**
 * @brief AI behaviour: wanderer
 */
void Entity::AIWander()
{
    // 1. If it collides with the left wall and is currently moving leftwards
    if (mIsCollidingLeft && mMovement.x < 0.0f)
    {
        moveRight(); // change direction
    }
    // 2. collides with the right wall
    else if (mIsCollidingRight && mMovement.x > 0.0f)
    {
        moveLeft();
    }
    else if (mMovement.x == 0.0f)
    {
        moveLeft();
    }
}

/**
 * @brief AI behaviour: follower
 */
void Entity::AIFollow(Entity *player)
{
    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, player->getPosition()) < 300.0f)
            mAIState = WALKING; // Switch to walking mode
        break;
    case WALKING:
        if (Vector2Distance(mPosition, player->getPosition()) > 400.0f)
        {
            mAIState = IDLE;
            resetMovement();
        }
        // follow logic
        else if (mPosition.x > player->getPosition().x)
            moveLeft();
        else
            moveRight();
        break;
    default:
        break;
    }
}

/**
 * @brief AI behaviour: Flyer (up and down)
 */
void Entity::AIFlyer()
{
    mMovement.y = sinf((float)GetTime() * 2.2f);
}

/**
 * @brief AABB collision detection
 */
bool Entity::isColliding(Entity *other) const
{
    if (mEntityStatus == INACTIVE || other->mEntityStatus == INACTIVE)
        return false;

    // collision detection
    float xDistance = fabs(mPosition.x - other->getPosition().x) - ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    return (xDistance < 0.0f && yDistance < 0.0f);
}

/**
 * @brief Check for collision between the Y-axis and the map
 */
void Entity::checkCollisionY(Map *map)
{
    if (map == nullptr)
        return;

    // ------ Probes ------
    // Top probe
    float horizontalIndent = 5.0f; // for slightly inward
    Vector2 topCentreProbe = {mPosition.x, mPosition.y - (mColliderDimensions.y / 2.0f)};
    Vector2 topLeftProbe = {mPosition.x - (mColliderDimensions.x / 2.0f) + horizontalIndent, mPosition.y - (mColliderDimensions.y / 2.0f)};
    Vector2 topRightProbe = {mPosition.x + (mColliderDimensions.x / 2.0f) - horizontalIndent, mPosition.y - (mColliderDimensions.y / 2.0f)};

    // Bottom probe
    Vector2 bottomCentreProbe = {mPosition.x, mPosition.y + (mColliderDimensions.y / 2.0f)};
    Vector2 bottomLeftProbe = {mPosition.x - (mColliderDimensions.x / 2.0f) + horizontalIndent, mPosition.y + (mColliderDimensions.y / 2.0f)};
    Vector2 bottomRightProbe = {mPosition.x + (mColliderDimensions.x / 2.0f) - horizontalIndent, mPosition.y + (mColliderDimensions.y / 2.0f)};

    float xOverlap = 0.0f, yOverlap = 0.0f;

    // 1. Check for upward collision (negative velocity)
    if (mVelocity.y < 0.0f && (map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap) || map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap) || map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap)))
    {
        mPosition.y += yOverlap;
        mVelocity.y = 0.0f;
        mIsCollidingTop = true;
    }

    // 2. Check for downward collision (with positive velocity)
    if (mVelocity.y > 0.0f && (map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) || map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap) || map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap)))
    {
        mPosition.y -= yOverlap;
        mVelocity.y = 0.0f;
        mIsCollidingBottom = true;
    }
}

/**
 * @brief Check for collision between the -axis and the map
 */
void Entity::checkCollisionX(Map *map)
{
    if (map == nullptr)
        return;

    // ------ Probes ------
    float verticalIndent = 5.0f; // vertical inward

    // Left probe
    Vector2 leftCentreProbe = {mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y};
    Vector2 leftTopProbe = {mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) + verticalIndent};
    Vector2 leftBottomProbe = {mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) - verticalIndent};

    // Right probe
    Vector2 rightCentreProbe = {mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y};
    Vector2 rightTopProbe = {mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) + verticalIndent};
    Vector2 rightBottomProbe = {mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) - verticalIndent};

    float xOverlap = 0.0f, yOverlap = 0.0f;

    // 1. Check for left collision (negative velocity)
    if (mVelocity.x < 0.0f && (map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap) || map->isSolidTileAt(leftTopProbe, &xOverlap, &yOverlap) || map->isSolidTileAt(leftBottomProbe, &xOverlap, &yOverlap)))
    {
        mPosition.x += xOverlap;
        mVelocity.x = 0.0f;
        mIsCollidingLeft = true;
    }

    // 2. Check for rightward collision (with positive velocity)
    if (mVelocity.x > 0.0f && (map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap) || map->isSolidTileAt(rightTopProbe, &xOverlap, &yOverlap) || map->isSolidTileAt(rightBottomProbe, &xOverlap, &yOverlap)))
    {
        mPosition.x -= xOverlap;
        mVelocity.x = 0.0f;
        mIsCollidingRight = true;
    }
}

/**
 * @brief Entity's update function
 */
void Entity::update(float deltaTime, Entity *player, Map *map, Entity *collidableEntities, int collisionCheckCount)
{
    if (mEntityStatus == INACTIVE)
        return;

    if (mEntityType == NPC)
        AIActivate(player);

    resetColliderFlags();

    // Computing speed
    mVelocity.x = mMovement.x * mSpeed;
    if (mAIType != FLYER)
        mVelocity.y += mAcceleration.y * deltaTime;
    else
        mVelocity.y = mMovement.y * mSpeed; // for flyer

    // for jump
    if (mIsJumping && mAIType != FLYER)
    {
        mIsJumping = false;
        mVelocity.y -= mJumpingPower;
    }

    // Collision resolution
    // Y-axis
    mPosition.y += mVelocity.y * deltaTime;
    checkCollisionY(map);

    // X-axis
    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(map);

    // animation will only play when the player is on the ground and moving.)
    if (mTextureType == ATLAS && mEntityType == PLAYER && GetLength(mMovement) != 0 && mIsCollidingBottom)
        animate(deltaTime);
}

/**
 * @brief Animation frame refresh
 */
void Entity::animate(float deltaTime)
{
    if (mAnimationAtlas.empty() || mAnimationAtlas.find(mDirection) == mAnimationAtlas.end())
        return;

    // Retrieve the list of animation frame indices based on the current direction
    mAnimationIndices = mAnimationAtlas.at(mDirection);
    if (mAnimationIndices.empty())
        return;

    // Cumulatived time
    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;
        mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mAnimationIndices.size();
    }
}

/**
 * @brief render Entity
 */
void Entity::render()
{
    if (mEntityStatus == INACTIVE)
        return;

    Rectangle textureArea; // UV cord

    if (mTextureType == SINGLE)
    {
        textureArea = {0.0f, 0.0f, (float)mTexture.width, (float)mTexture.height};
    }
    else // (mTextureType == ATLAS)
    {
        if (mAnimationIndices.empty())
            textureArea = getUVRectangle(&mTexture, 0, mSpriteSheetDimensions.x, mSpriteSheetDimensions.y);
        else
            textureArea = getUVRectangle(&mTexture, mAnimationIndices[mCurrentFrameIndex], mSpriteSheetDimensions.x, mSpriteSheetDimensions.y);
    }

    Rectangle destinationArea = {mPosition.x, mPosition.y, mScale.x, mScale.y};
    Vector2 originOffset = {mScale.x / 2.0f, mScale.y / 2.0f};

    DrawTexturePro(mTexture, textureArea, destinationArea, originOffset, mAngle, WHITE);

    // --- test ---
    // displayCollider();
}

/**
 * @brief Debugging: Drawing collision box
 */
void Entity::displayCollider()
{
    DrawRectangleLines(
        mPosition.x - mColliderDimensions.x / 2.0f,
        mPosition.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x,
        mColliderDimensions.y,
        GREEN);
}