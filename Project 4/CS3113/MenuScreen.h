#pragma once
#include "Scene.h"

// Menu Screen Class, inherite from Scene
class MenuScreen : public Scene
{
private:
    const char *mMainText;
    const char *mSubText;
    SceneID mNextScene; // next scene ID

public:
    MenuScreen(const char *mainText, const char *subText, SceneID nextScene);
    ~MenuScreen() override = default;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};