#include "Map.h"

Map::Map(int mapColumns, int mapRows, unsigned int *levelData,
         const char *textureFilePath, float tileSize, int textureColumns,
         int textureRows, Vector2 origin) : mMapColumns{mapColumns}, mMapRows{mapRows},
                                            mTextureAtlas{LoadTexture(textureFilePath)},
                                            mLevelData{levelData}, mTileSize{tileSize},
                                            mTextureColumns{textureColumns}, mTextureRows{textureRows},
                                            mOrigin{origin}
{
    // Construct the map immediately
    build();
}

Map::~Map()
{
    UnloadTexture(mTextureAtlas);
}

/**
 * @brief Construct map
 */
void Map::build()
{
    // 1. Calculate the boundaries of the map
    mLeftBoundary = mOrigin.x - (mMapColumns * mTileSize) / 2.0f;
    mRightBoundary = mOrigin.x + (mMapColumns * mTileSize) / 2.0f;
    mTopBoundary = mOrigin.y - (mMapRows * mTileSize) / 2.0f;
    mBottomBoundary = mOrigin.y + (mMapRows * mTileSize) / 2.0f;

    // 2. Compute the UV rectangles for each tile
    for (int row = 0; row < mTextureRows; row++)
    {
        for (int col = 0; col < mTextureColumns; col++)
        {
            Rectangle textureArea = {
                (float)col * (mTextureAtlas.width / mTextureColumns),
                (float)row * (mTextureAtlas.height / mTextureRows),
                (float)mTextureAtlas.width / mTextureColumns,
                (float)mTextureAtlas.height / mTextureRows};

            mTextureAreas.push_back(textureArea);
        }
    }
}

/**
 * @brief Render the entire map
 */
void Map::render()
{
    // Iterate through each row
    for (int row = 0; row < mMapRows; row++)
    {
        // Iterate through each column
        for (int col = 0; col < mMapColumns; col++)
        {
            int tile = mLevelData[row * mMapColumns + col];

            // skip
            if (tile == 0)
                continue;

            Rectangle destinationArea = {
                mLeftBoundary + col * mTileSize,
                mTopBoundary + row * mTileSize,
                mTileSize,
                mTileSize};

            DrawTexturePro(
                mTextureAtlas,
                mTextureAreas[tile - 1],
                destinationArea,
                {0.0f, 0.0f},
                0.0f,
                WHITE);
        }
    }
}

bool Map::isSolidTileAt(Vector2 position, float *xOverlap, float *yOverlap)
{

    *xOverlap = 0.0f;
    *yOverlap = 0.0f;

    // Check if position is outside map boundaries
    if (position.x < mLeftBoundary || position.x > mRightBoundary ||
        position.y < mTopBoundary || position.y > mBottomBoundary)
        return false;

    int tileXIndex = floor((position.x - mLeftBoundary) / mTileSize);
    int tileYIndex = floor((position.y - mTopBoundary) / mTileSize);

    if (tileXIndex < 0 || tileXIndex >= mMapColumns ||
        tileYIndex < 0 || tileYIndex >= mMapRows)
        return false;

    // Get tile ID at calculated indices
    int tile = mLevelData[tileYIndex * mMapColumns + tileXIndex];

    if (tile == 0)
        return false;

    // Calculate the center position
    float tileCentreX = mLeftBoundary + tileXIndex * mTileSize + mTileSize / 2.0f;
    float tileCentreY = mTopBoundary + tileYIndex * mTileSize + mTileSize / 2.0f;

    // Calculate overlap amounts
    *xOverlap = fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.x - tileCentreX));
    *yOverlap = fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.y - tileCentreY));

    return true;
}