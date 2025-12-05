#include "Effects.h"

Effects::Effects(Vector2 origin, float windowWidth, float windowHeight)
    : mAlpha(SOLID),
      mEffectSpeed(DEFAULT_SPEED),
      mOverlayWidth(windowWidth),
      mOverlayHeight(windowHeight),
      mCurrentEffect(NONE),
      mViewOffset({0.0f, 0.0f}),
      mOrigin(origin),
      mMaxWindowDimensions({windowWidth, windowHeight})
{
}

void Effects::drawOverlay()
{
    float left = mViewOffset.x - mOverlayWidth / 2.0f;
    float top = mViewOffset.y - mOverlayHeight / 2.0f;

    DrawRectangle((int)left,
                  (int)top,
                  (int)mOverlayWidth,
                  (int)mOverlayHeight,
                  Fade(BLACK, mAlpha));
}

void Effects::start(EffectType effectType)
{
    mCurrentEffect = effectType;

    switch (mCurrentEffect)
    {
    case FADEIN:
        mAlpha = SOLID; // Start solid black, ready to fade to transparent
        break;

    case FADEOUT:
        mAlpha = TRANSPARENT;
        break;

    case SHRINK:
        mOverlayHeight = mMaxWindowDimensions.y;
        mOverlayWidth = mMaxWindowDimensions.x;
        break;

    case GROW:
        mOverlayHeight = 0.0f;
        mOverlayWidth = 0.0f;
        break;

    case NONE:
    default:
        break;
    }
}

void Effects::update(float deltaTime, Vector2 *viewOffset)
{
    if (viewOffset != nullptr)
        mViewOffset = *viewOffset;

    switch (mCurrentEffect)
    {
    case FADEIN:
        mAlpha -= mEffectSpeed * deltaTime; // Alpha decreases -> Black fades away -> Game scene revealed

        if (mAlpha <= TRANSPARENT)
        {
            mAlpha = TRANSPARENT;
            mCurrentEffect = NONE;
        }
        break;

    case FADEOUT:
        mAlpha += mEffectSpeed * deltaTime;

        if (mAlpha >= SOLID)
        {
            mAlpha = SOLID;
            mCurrentEffect = NONE;
        }
        break;

    case NONE:
    default:
        break;
    }
}

void Effects::render()
{
    if (mCurrentEffect != NONE)
    {
        drawOverlay();
    }
}