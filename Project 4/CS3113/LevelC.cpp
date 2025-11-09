#include "LevelC.h"

// Level C Constants
constexpr int LEVEL_WIDTH_C = 40;
constexpr int LEVEL_HEIGHT_C = 12;

// Map data for Level C (0 = empty, 1=tile 1, 2=tile 2, 3=tile 3)
constexpr unsigned int LEVEL_DATA_C[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
    3, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 3, 0,
    3, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 3, 0,
    3, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 3, 0,
    3, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 3, 0};

/**
 * @brief Initialise Level C
 */
void LevelC::initialise()
{
   mGameState.nextSceneID = -1;

   // Loading music and sound effects
   mGameState.bgm = LoadMusicStream("assets/game/blackground.mp3");
   PlayMusicStream(mGameState.bgm);
   SetMusicVolume(mGameState.bgm, 0.7f);
   mGameState.jumpSound = LoadSound("assets/game/jump.ogg");
   mGameState.bumpSound = LoadSound("assets/game/bump.ogg");

   // Create a map instance
   mGameState.map = new Map(LEVEL_WIDTH_C, LEVEL_HEIGHT_C, (unsigned int *)LEVEL_DATA_C, "assets/game/tileset.png", 50.0f, 3, 1, mOrigin);

   // Create a player instance
   std::map<Direction, std::vector<int>> anim = {{DOWN, {0, 4, 8, 12}}, {LEFT, {1, 5, 9, 13}}, {UP, {2, 6, 10, 14}}, {RIGHT, {3, 7, 11, 15}}};
   mGameState.player = new Entity({100.0f, 300.0f}, {80.0f, 80.0f}, "assets/game/george.png", ATLAS, {4, 4}, anim, PLAYER);

   // Set the player's physical attributes
   mGameState.player->setJumpingPower(600.0f);
   mGameState.player->setAcceleration({0.0f, 981.0f});
   mGameState.player->setColliderDimensions({25.0f, 50.0f});

   // Create AI
   mGameState.enemy = new Entity({1400.0f, 150.0f}, {50.0f, 50.0f}, "assets/game/flyer.png", NPC);
   mGameState.enemy->setAIType(FLYER);
   mGameState.enemy->setAIState(FLYING);
   mGameState.enemy->setAcceleration({0.0f, 0.0f});
   mGameState.enemy->setSpeed(80);
   mGameState.enemy->setColliderDimensions({30.0f, 30.0f});

   // Initialise Camera
   mGameState.camera = {0};
   mGameState.camera.target = {200.0f, 300.0f};
   mGameState.player->setPosition({200.0f, 300.0f});
   mGameState.camera.offset = mOrigin;
   mGameState.camera.zoom = 1.0f;
}

/**
 * @brief Update Level C
 * @param deltaTime
 */
void LevelC::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.player->update(deltaTime, mGameState.player, mGameState.map, nullptr, 0);
   mGameState.enemy->update(deltaTime, mGameState.player, mGameState.map, nullptr, 0);

   // Level transition logic
   if (mGameState.player->getPosition().x > mGameState.map->getRightBoundary() - 100.0f)
      mGameState.nextSceneID = SCENE_WIN; // Switch to the victory scene
}

/**
 * @brief Render Level C
 */
void LevelC::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   mGameState.map->render();
   mGameState.player->render();
   mGameState.enemy->render();
}

void LevelC::shutdown()
{
   delete mGameState.player;
   delete mGameState.map;
   delete mGameState.enemy;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.bumpSound);
}