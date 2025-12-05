#include "MenuScreen.h"

MenuScreen::MenuScreen(const char *mainText, const char *subText, SceneID nextScene)
    : mMainText(mainText), mSubText(subText), mNextScene(nextScene), mStoryText(nullptr), mControlText(nullptr) {}

MenuScreen::MenuScreen(const char *mainText, const char *subText, const char *storyText, const char *controlText, SceneID nextScene)
    : mMainText(mainText), mSubText(subText), mStoryText(storyText), mControlText(controlText), mNextScene(nextScene) {}

void MenuScreen::initialise()
{
   mGameState.nextSceneID = -1;
}

void MenuScreen::update(float deltaTime)
{
   if (IsKeyPressed(KEY_ENTER))
   {
      mGameState.nextSceneID = mNextScene;
   }
}

void MenuScreen::render()
{
   ClearBackground(ColorFromHex("#2C3E50"));
   int screenWidth = GetScreenWidth();
   int screenHeight = GetScreenHeight();

   int fontSizeMain = 50;
   DrawText(mMainText, screenWidth / 2 - MeasureText(mMainText, fontSizeMain) / 2, screenHeight / 6, fontSizeMain, WHITE);

   int fontSizeSub = 24;
   if ((int)(GetTime() * 2) % 2 == 0)
   {
      DrawText(mSubText, screenWidth / 2 - MeasureText(mSubText, fontSizeSub) / 2, screenHeight / 6 + 60, fontSizeSub, YELLOW);
   }

   if (mStoryText != nullptr)
   {
      DrawText(mStoryText, 100, screenHeight / 2 - 80, 20, WHITE);
   }
   if (mControlText != nullptr)
   {
      DrawText(mControlText, 100, screenHeight / 2 + 120, 20, GREEN);
   }
}

void MenuScreen::shutdown() {}