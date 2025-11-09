#ifndef MAP_H
#define MAP_H

#include "cs3113.h"

class Map
{
private:
    int mMapColumns; // Number of col in the map
    int mMapRows;
    unsigned int *mLevelData;
    Texture2D mTextureAtlas;

    float mTileSize;

    int mTextureColumns; // Number of columns in the texture atlas
    int mTextureRows;

    std::vector<Rectangle> mTextureAreas; // UV cord
    Vector2 mOrigin;                      // Map centre point

    // Map Boundary
    float mLeftBoundary;
    float mRightBoundary;
    float mTopBoundary;
    float mBottomBoundary;

public:
    Map(int mapColumns, int mapRows, unsigned int *levelData,
        const char *textureFilePath, float tileSize, int textureColumns,
        int textureRows, Vector2 origin);
    ~Map();

    void build();
    void render();

    /**
     * @brief Check world coordinates
     * @param position
     * @param xOverlap overlap amount along the X-axis.
     * @param yOverlap overlap amount along the Y-axis.
     */
    bool isSolidTileAt(Vector2 position, float *xOverlap, float *yOverlap);

    // Getters
    int getMapColumns() const { return mMapColumns; };
    int getMapRows() const { return mMapRows; };
    float getTileSize() const { return mTileSize; };
    unsigned int *getLevelData() const { return mLevelData; };
    Texture2D getTextureAtlas() const { return mTextureAtlas; };
    int getTextureColumns() const { return mTextureColumns; };
    int getTextureRows() const { return mTextureRows; };
    float getLeftBoundary() const { return mLeftBoundary; };
    float getRightBoundary() const { return mRightBoundary; };
    float getTopBoundary() const { return mTopBoundary; };
    float getBottomBoundary() const { return mBottomBoundary; };
};

#endif // MAP_H