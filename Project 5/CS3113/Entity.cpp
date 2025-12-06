#include "Entity.h"
#include "Map.h"
#include "cs3113.h"

Entity::Entity()
    : mPosition{0.0f, 0.0f}, mMovement{0.0f, 0.0f}, mScale{1.0f, 1.0f}, mEntityType(PROP),
      mSpeed(0.0f), mAnimationAtlas(), mFrameSpeed(6), mTextureType(SINGLE)
{
}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType)
    : mPosition(position), mMovement{0.0f, 0.0f}, mScale(scale), mEntityType(entityType),
      mSpeed(0.0f), mAnimationAtlas(), mFrameSpeed(6), mTextureType(SINGLE)
{
    mTexture = LoadTexture(textureFilepath);
}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction, std::vector<int>> animationAtlas, EntityType entityType)
    : mPosition(position), mMovement{0.0f, 0.0f}, mScale(scale), mEntityType(entityType),
      mSpeed(0.0f), mAnimationAtlas(animationAtlas), mFrameSpeed(6), mTextureType(textureType), mSpriteSheetDimensions(spriteSheetDimensions)
{
    mTexture = LoadTexture(textureFilepath);
    if (mAnimationAtlas.count(DOWN) > 0)
    {
        mAnimationIndices = mAnimationAtlas[DOWN];
    }
}

Entity::~Entity() {}

// Calculate attack hitbox
Rectangle Entity::getAttackBox() const
{
    float range = (mEntityType == PLAYER) ? 9.0f : 5.0f;
    float width = (mEntityType == PLAYER) ? 19.0f : 18.0f;

    // Boss has a larger attack range
    if (mAIType == DEMON && mEntityType == ENEMY)
    {
        range = 15.0f;
        width = 30.0f;
    }

    float x = mPosition.x;
    float y = mPosition.y;

    // Generate a rectangle in front based on direction
    switch (mDirection)
    {
    case LEFT:
        return {x - mColliderDimensions.x / 2.0f - range, y - width / 2.0f, range, width};
    case RIGHT:
        return {x + mColliderDimensions.x / 2.0f, y - width / 2.0f, range, width};
    case UP:
        return {x - width / 2.0f, y - mColliderDimensions.y / 2.0f - range, width, range};
    case DOWN:
        return {x - width / 2.0f, y + mColliderDimensions.y / 2.0f, width, range};
    default:
        return {0, 0, 0, 0};
    }
}

// Draw collision boxes for debugging
void Entity::displayCollider()
{
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,
        mPosition.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x,
        mColliderDimensions.y};

    DrawRectangleLines(colliderBox.x, colliderBox.y, colliderBox.width, colliderBox.height, GREEN);

    if (mAIState == ATTACKING)
    {
        Rectangle attackBox = getAttackBox();
        DrawRectangleLines(attackBox.x, attackBox.y, attackBox.width, attackBox.height, RED);
    }
}

// Main Entity update loop
void Entity::update(float deltaTime, Entity *player, Map *map, std::vector<Entity *> *obstacles)
{
    if (mEntityStatus == INACTIVE || mAIState == DEAD)
        return;

    if (mEntityType == ENEMY)
        aiActivate(player, deltaTime);

    animate(deltaTime);

    if (mAIState == DYING)
        return;

    // Cannot move while attacking
    if (mAIState == ATTACKING)
    {
        mMovement = {0.0f, 0.0f};
    }

    mVelocity.x = mMovement.x * mSpeed;
    mVelocity.y = mMovement.y * mSpeed;

    // X-axis movement and collision handling
    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(map);
    if (obstacles)
        checkCollisionWithObstacles(obstacles);

    // Y-axis movement and collision handling
    mPosition.y += mVelocity.y * deltaTime;
    checkCollisionY(map);
    if (obstacles)
        checkCollisionWithObstacles(obstacles);
}

void Entity::checkCollisionY(Map *map)
{
    Vector2 top = {mPosition.x, mPosition.y - mColliderDimensions.y / 2};
    Vector2 bottom = {mPosition.x, mPosition.y + mColliderDimensions.y / 2};
    float xOver = 0.0f, yOver = 0.0f;

    if (mVelocity.y < 0 && map->isSolid(top, &xOver, &yOver))
    {
        mPosition.y += yOver;
        mVelocity.y = 0;
    }
    else if (mVelocity.y > 0 && map->isSolid(bottom, &xOver, &yOver))
    {
        mPosition.y -= yOver;
        mVelocity.y = 0;
    }
}

void Entity::checkCollisionX(Map *map)
{
    Vector2 left = {mPosition.x - mColliderDimensions.x / 2, mPosition.y};
    Vector2 right = {mPosition.x + mColliderDimensions.x / 2, mPosition.y};
    float xOver = 0.0f, yOver = 0.0f;

    if (mVelocity.x < 0 && map->isSolid(left, &xOver, &yOver))
    {
        mPosition.x += xOver;
        mVelocity.x = 0;
    }
    else if (mVelocity.x > 0 && map->isSolid(right, &xOver, &yOver))
    {
        mPosition.x -= xOver;
        mVelocity.x = 0;
    }
}

// Check collision with obstacles (other entities) and correct position
void Entity::checkCollisionWithObstacles(std::vector<Entity *> *obstacles)
{
    for (Entity *obj : *obstacles)
    {
        if (obj == this || obj->getAIState() == DEAD)
            continue;

        if (isColliding(obj))
        {
            // Calculate overlap amount
            float xDist = fabs(mPosition.x - obj->getPosition().x);
            float yDist = fabs(mPosition.y - obj->getPosition().y);
            float xOverlap = (mColliderDimensions.x + obj->getColliderDimensions().x) / 2.0f - xDist;
            float yOverlap = (mColliderDimensions.y + obj->getColliderDimensions().y) / 2.0f - yDist;

            // Push apart on the axis with smaller overla
            if (xOverlap < yOverlap)
            {
                if (mPosition.x < obj->getPosition().x)
                    mPosition.x -= xOverlap;
                else
                    mPosition.x += xOverlap;
            }
            else
            {
                if (mPosition.y < obj->getPosition().y)
                    mPosition.y -= yOverlap;
                else
                    mPosition.y += yOverlap;
            }
        }
    }
}

bool Entity::isColliding(Entity *other) const
{
    if (other->getAIState() == DEAD)
        return false;

    float xDist = fabs(mPosition.x - other->mPosition.x);
    float yDist = fabs(mPosition.y - other->mPosition.y);
    return (xDist < (mColliderDimensions.x + other->mColliderDimensions.x) / 2.0f) &&
           (yDist < (mColliderDimensions.y + other->mColliderDimensions.y) / 2.0f);
}

void Entity::aiActivate(Entity *player, float deltaTime)
{
    if (mAIType == SOLDIER)
        aiSoldier(player);
    else if (mAIType == SLIME)
        aiSlime(player, deltaTime);
    else if (mAIType == DEMON)
        aiDemon(player, deltaTime); // input deltaTime
}

void Entity::aiSlime(Entity *player, float deltaTime)
{
    if (mAIState == DEAD || mAIState == DYING || mAIState == ATTACKING)
        return;

    float distToPlayer = Vector2Distance(mPosition, player->getPosition());

    // If very close to player, initiate attack
    if (distToPlayer < 25.0f)
    {
        mAIState = ATTACKING;
        mMovement = {0.0f, 0.0f};

        float dx = player->getPosition().x - mPosition.x;
        float dy = player->getPosition().y - mPosition.y;
        if (fabs(dx) > fabs(dy))
            mDirection = (dx > 0) ? RIGHT : LEFT;
        else
            mDirection = (dy > 0) ? DOWN : UP;

        mCurrentFrameIndex = 0;
        mHasDealtDamage = false;
        return;
    }

    // If player enters alert range, provoke
    if (distToPlayer < 150.0f)
    {
        mIsProvoked = true;
    }

    if (mIsProvoked)
    {
        aiChase(player);
        mSpeed = 60.0f;
        // Stop chasing if player runs far away
        if (distToPlayer > 300.0f)
        {
            mIsProvoked = false;
        }
    }
    else
    {
        // Circular patrol logic
        mSpeed = 40.0f;
        mPatrolAngle += 1.0f * deltaTime;
        if (mPatrolAngle > 2 * PI)
            mPatrolAngle -= 2 * PI;

        float targetX = mPatrolCenter.x + cosf(mPatrolAngle) * mPatrolRadius;
        float targetY = mPatrolCenter.y + sinf(mPatrolAngle) * mPatrolRadius;
        Vector2 targetPos = {targetX, targetY};

        Vector2 direction = Vector2Subtract(targetPos, mPosition);
        if (Vector2Length(direction) > 1.0f)
        {
            mMovement = Vector2Normalize(direction);
            if (fabs(mMovement.x) > fabs(mMovement.y))
                mDirection = (mMovement.x > 0) ? RIGHT : LEFT;
            else
                mDirection = (mMovement.y > 0) ? DOWN : UP;
        }
        else
        {
            mMovement = {0, 0};
        }
    }
}

void Entity::aiDemon(Entity *player, float deltaTime)
{
    if (mAIState == DEAD || mAIState == DYING || mAIState == ATTACKING)
        return;

    // Cooldown check: If still cooling down, stay put
    if (mAttackCooldown > 0.0f)
    {
        mAttackCooldown -= deltaTime;
        mMovement = {0.0f, 0.0f}; // cease move
        return;
    }

    float distToPlayer = Vector2Distance(mPosition, player->getPosition());

    // attack Check
    if (distToPlayer < 40.0f)
    {
        mAIState = ATTACKING;
        mMovement = {0.0f, 0.0f};

        float dx = player->getPosition().x - mPosition.x;
        float dy = player->getPosition().y - mPosition.y;
        if (fabs(dx) > fabs(dy))
            mDirection = (dx > 0) ? RIGHT : LEFT;
        else
            mDirection = (dy > 0) ? DOWN : UP;

        mCurrentFrameIndex = 0;
        mHasDealtDamage = false;
        return;
    }

    // Chase
    aiChase(player);
    mSpeed = 80.0f;
}

void Entity::aiSoldier(Entity *player)
{
    if (mAIState == DEAD || mAIState == DYING || mAIState == ATTACKING)
        return;

    if (mIsProvoked)
    {
        if (Vector2Distance(mPosition, player->getPosition()) < 25.0f)
        {
            mAIState = ATTACKING;
            mMovement = {0.0f, 0.0f};

            float dx = player->getPosition().x - mPosition.x;
            float dy = player->getPosition().y - mPosition.y;

            if (fabs(dx) > fabs(dy))
                mDirection = (dx > 0) ? RIGHT : LEFT;
            else
                mDirection = (dy > 0) ? DOWN : UP;

            mCurrentFrameIndex = 0;
            mHasDealtDamage = false;
        }
        else
        {
            aiChase(player);
            mSpeed = 70.0f;
        }
    }
    else
    {
        aiPatrol();
        mSpeed = 30.0f;
    }
}

// Random movement patrol
void Entity::aiPatrol()
{
    int change = GetRandomValue(0, 100);
    if (change < 2)
    {
        int dir = GetRandomValue(0, 4);
        mMovement = {0, 0};
        if (dir == 0)
            moveUp();
        else if (dir == 1)
            moveDown();
        else if (dir == 2)
            moveLeft();
        else if (dir == 3)
            moveRight();
    }
    normaliseMovement();
}

// Simple chase logic (Move directly towards player coordinates)
void Entity::aiChase(Entity *player)
{
    if (Vector2Distance(mPosition, player->getPosition()) < 20.0f)
    {
        mMovement = {0, 0};
        return;
    }

    mMovement = {0, 0};

    float xDiff = player->getPosition().x - mPosition.x;
    float yDiff = player->getPosition().y - mPosition.y;

    if (xDiff > 0)
        mMovement.x = 1.0f;
    else if (xDiff < 0)
        mMovement.x = -1.0f;

    if (yDiff > 0)
        mMovement.y = 1.0f;
    else if (yDiff < 0)
        mMovement.y = -1.0f;

    // Set direction
    if (fabs(xDiff) > fabs(yDiff))
    {
        if (xDiff > 0)
            mDirection = RIGHT;
        else
            mDirection = LEFT;
    }
    else
    {
        if (yDiff > 0)
            mDirection = DOWN;
        else
            mDirection = UP;
    }

    normaliseMovement();
}

void Entity::takeDamage(int damage)
{
    if (mAIState == DEAD || mAIState == DYING)
        return;
    mHealth -= damage;
    mIsProvoked = true;
    if (mHealth <= 0)
    {
        mAIState = DYING;
        mMovement = {0.0f, 0.0f};
    }
}

void Entity::normaliseMovement()
{
    if (Vector2Length(mMovement) > 0)
        mMovement = Vector2Normalize(mMovement);
}

void Entity::setTexture(Texture2D newTexture, int cols, int rows, std::map<Direction, std::vector<int>> newAtlas)
{
    if (mTexture.id != newTexture.id)
    {
        mTexture = newTexture;
        mSpriteSheetDimensions = {(float)cols, (float)rows};
        mAnimationAtlas = newAtlas;
        mCurrentFrameIndex = 0;
        mAnimationTime = 0.0f;
    }
}

void Entity::render()
{
    if (mEntityStatus == INACTIVE || mAIState == DEAD)
        return;

    Rectangle sourceRec;
    if (mTextureType == ATLAS)
    {
        float frameWidth = mTexture.width / mSpriteSheetDimensions.x;
        float frameHeight = mTexture.height / mSpriteSheetDimensions.y;
        if (mAnimationIndices.empty())
        {
            sourceRec = {0, 0, frameWidth, frameHeight};
        }
        else
        {
            // Calculate position of current frame in SpriteSheet
            int currentFrame = mAnimationIndices[mCurrentFrameIndex];
            sourceRec = {
                (currentFrame % (int)mSpriteSheetDimensions.x) * frameWidth,
                (currentFrame / (int)mSpriteSheetDimensions.x) * frameHeight,
                frameWidth, frameHeight};
        }
    }
    else
    {
        sourceRec = {0.0f, 0.0f, (float)mTexture.width, (float)mTexture.height};
    }
    Rectangle destRec = {mPosition.x, mPosition.y, mScale.x, mScale.y};
    Vector2 origin = {mScale.x / 2.0f, mScale.y / 2.0f};
    DrawTexturePro(mTexture, sourceRec, destRec, origin, mAngle, WHITE);

    displayCollider();
}

void Entity::animate(float deltaTime)
{
    if (mAnimationAtlas.empty() || mAnimationAtlas.find(mDirection) == mAnimationAtlas.end())
        return;
    mAnimationIndices = mAnimationAtlas.at(mDirection);
    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;
        mCurrentFrameIndex++;
        if (mAnimationIndices.empty())
            return;

        if (mAIState == ATTACKING)
        {
            // Attack animation doesn't loop, switch back to IDLE after playing once
            if (mCurrentFrameIndex >= mAnimationIndices.size())
            {
                mCurrentFrameIndex = 0;
                mAIState = IDLE;
                mHasDealtDamage = false;

                // If it is a demon, it enters cooldown after attacking.
                if (mAIType == DEMON)
                {
                    mAttackCooldown = 2.0f; // 2 Seconds Cooldown
                }
            }
        }
        else if (mAIState == DYING)
        {
            // Death animation stops at the last frame
            if (mCurrentFrameIndex >= mAnimationIndices.size())
            {
                mCurrentFrameIndex = mAnimationIndices.size() - 1;
                mAIState = DEAD;
            }
        }
        else
        {
            // Loop animation (Walk/Idle)
            mCurrentFrameIndex %= mAnimationIndices.size();
        }
    }
}

void Entity::unloadTexture()
{
    UnloadTexture(mTexture);
}