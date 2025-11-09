#include "Scene.h"

// Default constructor
Scene::Scene() : mOrigin{0.0f, 0.0f} {}

Scene::Scene(Vector2 origin, const char *bgHexCode)
    : mOrigin{origin}, mBGColourHexCode{bgHexCode} {}