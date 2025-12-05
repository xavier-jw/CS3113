#pragma once
#include "raylib.h"

// Enum defining the types of visual effects
enum EffectType
{
    NONE,
    FADEIN, // Fade in (Black -> Transparent)
    FADEOUT,
    SHRINK,
    GROW
};

class Effects
{
private:
    float mAlpha; // Alpha transparency value (0.0 - 1.0)
    float mEffectSpeed;
    float mOverlayWidth;
    float mOverlayHeight;
    EffectType mCurrentEffect; // Current effect state
    Vector2 mViewOffset;       // View offset
    Vector2 mOrigin;
    Vector2 mMaxWindowDimensions;

    void drawOverlay();

public:
    // Solid black
    static constexpr float SOLID = 1.0f;
    // Fully transparent
    static constexpr float TRANSPARENT = 0.0f;

    static constexpr float DEFAULT_SPEED = 0.50f;

    static constexpr float OVERLAY_MAX_SIZE = 1000.0f;
    static constexpr float SIZE_SPEED_MULTIPLIER = 100.0f;

    Effects(Vector2 origin, float windowWidth, float windowHeight);

    void start(EffectType effectType);

    // Update effect state (Calculate Alpha value)
    void update(float deltaTime, Vector2 *viewOffset);

    // Render the effect
    void render();
};