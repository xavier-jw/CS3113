#pragma once
#include "Scene.h"

/**
 * @class MenuScreen
 * @brief Represents a static screen in the game
 */
class MenuScreen : public Scene
{
public:
    // Constructor
    MenuScreen(const char *mainText, const char *subText, SceneID nextScene);
    MenuScreen(const char *mainText, const char *subText, const char *storyText, const char *controlText, SceneID nextScene);

    ~MenuScreen() override = default;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    const char *mMainText;
    const char *mSubText;
    const char *mStoryText;
    const char *mControlText;
    SceneID mNextScene;
};