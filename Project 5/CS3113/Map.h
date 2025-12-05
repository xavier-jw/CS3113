#pragma once
#include "cs3113.h"
#include <vector>

class Map
{
private:
    int mMapColumns; // Number of map columns
    int mMapRows;    // Number of map rows
    unsigned int *mLevelData;

    Texture2D mTextureAtlas;
    float mTileSize;
    int mTextureColumns;
    int mTextureRows;
    Vector2 mOrigin;

    float mLeftBoundary;
    float mRightBoundary;
    float mTopBoundary;
    float mBottomBoundary;

    std::vector<Rectangle> mTextureAreas;

public:
    Map(int mapColumns, int mapRows, unsigned int *levelData, const char *textureFilePath, float tileSize, int textureColumns, int textureRows, Vector2 origin);
    ~Map();

    void build();
    void render();
    bool isSolid(Vector2 position, float *xOverlap, float *yOverlap);
};