#include "Windmill.h"

Windmill::Windmill(Vector2 position)
    : Entity(
          position,
          {90.0f, 90.0f},             // Scale
          "assets/game/Windmill.png", // Texture
          ATLAS,                      // Type
          {6, 1},
          {{DOWN, {0, 1, 2, 3, 4, 5}}},
          PROP)
{
    setColliderDimensions({0.0f, 0.0f});
}

Windmill::~Windmill()
{
    unloadTexture();
}