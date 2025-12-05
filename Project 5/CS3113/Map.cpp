#include "Map.h"

Map::Map(int mapColumns, int mapRows, unsigned int *levelData,
         const char *textureFilePath, float tileSize, int textureColumns,
         int textureRows, Vector2 origin) : mMapColumns{mapColumns}, mMapRows{mapRows},
                                            mTextureAtlas{LoadTexture(textureFilePath)},
                                            mLevelData{levelData}, mTileSize{tileSize},
                                            mTextureColumns{textureColumns}, mTextureRows{textureRows},
                                            mOrigin{origin}
{
    build();
}

Map::~Map()
{
    UnloadTexture(mTextureAtlas);
}

void Map::build()
{
    // Calculate map world coordinate boundaries
    mLeftBoundary = mOrigin.x - (mMapColumns * mTileSize) / 2.0f;
    mRightBoundary = mOrigin.x + (mMapColumns * mTileSize) / 2.0f;
    mTopBoundary = mOrigin.y - (mMapRows * mTileSize) / 2.0f;
    mBottomBoundary = mOrigin.y + (mMapRows * mTileSize) / 2.0f;

    // Calculate map world coordinate boundaries
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

void Map::render()
{
    for (int row = 0; row < mMapRows; row++)
    {
        for (int col = 0; col < mMapColumns; col++)
        {
            // Get Tile ID for current grid
            int tile = mLevelData[row * mMapColumns + col];
            if (tile == 0)
                continue;

            // Calculate destination position for drawing
            Rectangle destinationArea = {
                mLeftBoundary + col * mTileSize,
                mTopBoundary + row * mTileSize,
                mTileSize, mTileSize};

            DrawTexturePro(mTextureAtlas, mTextureAreas[tile - 1], destinationArea, {0.0f, 0.0f}, 0.0f, WHITE);
        }
    }
}

bool Map::isSolid(Vector2 position, float *xOverlap, float *yOverlap)
{
    *xOverlap = 0.0f;
    *yOverlap = 0.0f;

    // 1. Check whether it exceeds the map boundaries
    if (position.x < mLeftBoundary || position.x > mRightBoundary ||
        position.y < mTopBoundary || position.y > mBottomBoundary)
        return false;

    // 2. Calculate the index of the current tile
    int tileXIndex = floor((position.x - mLeftBoundary) / mTileSize);
    int tileYIndex = floor((position.y - mTopBoundary) / mTileSize);

    // Border Protection
    if (tileXIndex < 0 || tileXIndex >= mMapColumns || tileYIndex < 0 || tileYIndex >= mMapRows)
        return false;

    // 3. Retrieve tile ID
    int tile = mLevelData[tileYIndex * mMapColumns + tileXIndex];

    bool isWalkable = true;

    if (isWalkable)
        return false;

    // Collision calculation code below, only executes if isWalkable = false
    float tileCentreX = mLeftBoundary + tileXIndex * mTileSize + mTileSize / 2.0f;
    float tileCentreY = mTopBoundary + tileYIndex * mTileSize + mTileSize / 2.0f;
    *xOverlap = fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.x - tileCentreX));
    *yOverlap = fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.y - tileCentreY));

    return true;
}