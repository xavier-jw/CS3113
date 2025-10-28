#include "Entity.h"

// Constructor and Destructor Function
// Default constructor
Entity::Entity() : mPosition{0.0f, 0.0f},
                   mVelocity{0.0f, 0.0f}, mAcceleration{0.0f, 0.0f},
                   mScale{DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions{DEFAULT_SIZE, DEFAULT_SIZE},
                   mTexture{}, mTextureType{SINGLE}, mAngle{0.0f},
                   mSpriteSheetDimensions{}, mDirection{RIGHT},
                   mAnimationAtlas{{}}, mAnimationIndices{}, mFrameSpeed{0} {}

// Constructor for SINGLE texture entities (like platform, bomb)
Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath) : mPosition{position}, mVelocity{0.0f, 0.0f}, mAcceleration{0.0f, 0.0f},
                                                                               mScale{scale}, mColliderDimensions{scale},
                                                                               mTexture{LoadTexture(textureFilepath)}, mTextureType{SINGLE},
                                                                               mDirection{RIGHT}, mAnimationAtlas{{}}, mAnimationIndices{},
                                                                               mFrameSpeed{0}, mAngle{0.0f} {}

// Constructor for ATLAS texture entities (like the player ship)
Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
               TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction, std::vector<int>> animationAtlas) : mPosition{position},
                                                                                                                               mVelocity{0.0f, 0.0f}, mAcceleration{0.0f, 0.0f},
                                                                                                                               mScale{scale}, mColliderDimensions{scale},
                                                                                                                               mTexture{LoadTexture(textureFilepath)}, mTextureType{ATLAS},
                                                                                                                               mSpriteSheetDimensions{spriteSheetDimensions},
                                                                                                                               mAnimationAtlas{animationAtlas}, mDirection{RIGHT},
                                                                                                                               // Set the default animation to the "DOWN" state (idle)
                                                                                                                               mAnimationIndices{animationAtlas.at(DOWN)},
                                                                                                                               mFrameSpeed{DEFAULT_FRAME_SPEED}, mAngle{0.0f}
{
}

// Destructor
Entity::~Entity()
{
    UnloadTexture(mTexture);
};

/**
 * @brief Checks for collision with another entity.
 * @param other A pointer to the other entity
 * @return true if the entities are overlapping, false otherwise.
 */
bool Entity::isColliding(Entity *other) const
{
    // Calculate the distance between the centers of the two entities on the x-axis,
    // then subtract half the width of each entity's collider
    float xDistance = fabs(mPosition.x - other->getPosition().x) -
                      ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);

    // for y-axis
    float yDistance = fabs(mPosition.y - other->getPosition().y) -
                      ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f)
        return true;

    return false;
}

/**
 * @brief Updates the entity's current animation frame based on delta time.
 * @param deltaTime
 */
void Entity::animate(float deltaTime)
{
    // Get the correct animation sequence (e.g., {0} for idle or {1,2...9} for thrust)
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;

    // Calculate how long each frame should be displayed (e.g., if mFrameSpeed is 12, this is 1.0 / 12.0 = 0.083 seconds per frame)
    float framesPerSecond = 1.0f / mFrameSpeed;

    // Check if enough time has passed to advance to the next frame
    if (mAnimationTime >= framesPerSecond)
    {
        // Reset the timer
        mAnimationTime = 0.0f;

        // Move to the next frame index
        mCurrentFrameIndex++;

        // Use the % operator to loop the animation back to the start
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

/**
 * @brief Renders the entity's collider box
 */
void Entity::displayCollider()
{
    // Define the rectangle for the collider box based on position and dimensions
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f, // Top-left X
        mPosition.y - mColliderDimensions.y / 2.0f, // Top-left Y
        mColliderDimensions.x,                      // Width
        mColliderDimensions.y                       // Height
    };

    DrawRectangleLines(
        colliderBox.x,
        colliderBox.y,
        colliderBox.width,
        colliderBox.height,
        GREEN);
}

/**
 * @brief Updates the entity's state (physics and animation).
 * @param deltaTime
 */
void Entity::update(float deltaTime)
{
    if (mEntityStatus == INACTIVE)
        return;

    // Update velocity based on acceleration (v = v0 + a*t)
    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    // Update position based on velocity (p = p0 + v*t)
    mPosition.y += mVelocity.y * deltaTime;
    mPosition.x += mVelocity.x * deltaTime;

    if (mTextureType == ATLAS)
    {
        // call the animate function to update its current frame.
        animate(deltaTime);
    }
}

/**
 * @brief Renders the entity to the screen.
 */
void Entity::render()
{
    if (mEntityStatus == INACTIVE)
        return;

    Rectangle textureArea;

    // Check what type of texture this entity uses
    switch (mTextureType)
    {
    case SINGLE:
        textureArea = {
            0.0f, 0.0f,
            static_cast<float>(mTexture.width),
            static_cast<float>(mTexture.height)};
        break;

    // If it's an ATLAS
    case ATLAS:
        textureArea = getUVRectangle(
            &mTexture,                             // texture atlas file
            mAnimationIndices[mCurrentFrameIndex], // specific frame
            mSpriteSheetDimensions.y,              // Total number of rows in the atlas (e.g., 1)
            mSpriteSheetDimensions.x               // Total number of columns in the atlas (e.g., 10)
        );
        break;

    default:
        break;
    }

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        mPosition.x,                  // Center X on screen
        mPosition.y,                  // Center Y on screen
        static_cast<float>(mScale.x), // Width on screen
        static_cast<float>(mScale.y)  // Height on screen
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f};

    // Render the texture on screen
    DrawTexturePro(
        mTexture,
        textureArea,
        destinationArea,
        originOffset,
        mAngle,
        WHITE);

    // Render the green debug collider box
    displayCollider();
}