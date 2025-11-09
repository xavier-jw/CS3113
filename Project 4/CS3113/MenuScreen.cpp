#include "MenuScreen.h"

MenuScreen::MenuScreen(const char *mainText, const char *subText, SceneID nextScene)
    : mMainText(mainText), mSubText(subText), mNextScene(nextScene) {}

/**
 * @brief Initialise Menu
 */
void MenuScreen::initialise()
{
   mGameState.nextSceneID = -1;
}

/**
 * @brief menu logic
 * @param deltaTime
 */
void MenuScreen::update(float deltaTime)
{
   if (IsKeyPressed(KEY_ENTER))
   {
      mGameState.nextSceneID = mNextScene;
   }
}

/**
 * @brief Render menu text
 */
void MenuScreen::render()
{
   ClearBackground(ColorFromHex("#2C3E50")); // 渲染背景

   int fontSizeMain = 40;
   int fontSizeSub = 20;

   DrawText(mMainText, GetScreenWidth() / 2 - MeasureText(mMainText, fontSizeMain) / 2, GetScreenHeight() / 2 - 50, fontSizeMain, WHITE);
   DrawText(mSubText, GetScreenWidth() / 2 - MeasureText(mSubText, fontSizeSub) / 2, GetScreenHeight() / 2 + 10, fontSizeSub, LIGHTGRAY);
}

void MenuScreen::shutdown()
{
}