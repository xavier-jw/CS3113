/**
 * Author: [Junjie Wang]
 * Assignment: Rise of the AI
 * Date due: 2025-11-08, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/
#include "CS3113/cs3113.h"
#include "CS3113/MenuScreen.h"
#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"

// Global Constants
constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600;
constexpr int FPS = 60;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;                                  // (60fps)
constexpr Vector2 ORIGIN = {(float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2}; // Centre of the screen

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f; // for fixed time steps

// Scene pointers
Scene *gCurrentScene = nullptr; // Pointto the current active scene
MenuScreen *gMenuScene = nullptr;
LevelA *gLevelAScene = nullptr;
LevelB *gLevelBScene = nullptr;
LevelC *gLevelCScene = nullptr;
MenuScreen *gWinScene = nullptr; // Win Scene
MenuScreen *gLoseScene = nullptr;

int gLives = 3; // Player health lives
Sound gLoseSound;

/**
 * @brief Switch to the specified scene
 * @param id Target scene ID
 */
void switchToScene(SceneID id)
{
    switch (id)
    {
    case SCENE_MENU:
        gCurrentScene = gMenuScene;
        break;
    case SCENE_LEVEL_A:
        gCurrentScene = gLevelAScene;
        break;
    case SCENE_LEVEL_B:
        gCurrentScene = gLevelBScene;
        break;
    case SCENE_LEVEL_C:
        gCurrentScene = gLevelCScene;
        break;
    case SCENE_WIN:
        gCurrentScene = gWinScene;
        break;
    case SCENE_LOSE:
        gCurrentScene = gLoseScene;
        break;
    }
    // Initialise new scene
    gCurrentScene->initialise();
}

/**
 * @brief Initialise Game
 */
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 4: Rise of the AI");
    InitAudioDevice();

    gLoseSound = LoadSound("assets/game/lose.ogg");

    // Instantiate all scenes
    gMenuScene = new MenuScreen("RISE OF THE AI", "Press Enter to Start", SCENE_LEVEL_A);
    gLevelAScene = new LevelA(ORIGIN, "#be8b80ff");
    gLevelBScene = new LevelB(ORIGIN, "#5e7eaeff");
    gLevelCScene = new LevelC(ORIGIN, "#202036ff");
    gWinScene = new MenuScreen("You Win!", "Press Enter to Main Menu", SCENE_MENU);
    gLoseScene = new MenuScreen("You Lose!", "Press Enter to Main Menu", SCENE_MENU);

    // Default into the menu scene
    switchToScene(SCENE_MENU);
    SetTargetFPS(FPS);
}

/**
 * @brief Process the input for each frame
 */
void processInput()
{
    // Exit the game
    if (IsKeyPressed(KEY_Q) || WindowShouldClose())
        gAppStatus = TERMINATED;

    // Retrieve the player object for the current scene
    Entity *player = gCurrentScene->getState().player;
    if (player)
    {
        player->resetMovement();

        // Movement
        if (IsKeyDown(KEY_A))
            player->moveLeft();
        else if (IsKeyDown(KEY_D))
            player->moveRight();

        // Jump
        if (IsKeyPressed(KEY_W) && player->isCollidingBottom())
        {
            player->jump();
            PlaySound(gCurrentScene->getState().jumpSound); // Play jump sound effect
        }
        if (GetLength(player->getMovement()) > 1.0f)
            player->normaliseMovement();
    }
}

/**
 * @brief Update the game world
 */
void update()
{
    // fixed time steps
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gTimeAccumulator += deltaTime;

    while (gTimeAccumulator >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);

        GameState &state = gCurrentScene->getState();

        if (state.nextSceneID != -1)
        {
            // If returning to the main menu, reset health points.
            if (state.nextSceneID == SCENE_MENU)
                gLives = 3;
            switchToScene((SceneID)state.nextSceneID);
        }

        // Player Death Logic
        if (state.player && state.enemy)
        {
            bool hasDied = false;

            // 1. Death by falling
            if (state.player->getPosition().y > SCREEN_HEIGHT + 200.0f)
            {
                hasDied = true;
            }
            // 2. Deathby colliding with enemies
            else if (state.player->isColliding(state.enemy))
            {
                PlaySound(state.bumpSound); // Play bump sound effect
                hasDied = true;
            }

            // Death outcomes
            if (hasDied)
            {
                gLives--; // reduce lives
                if (gLives > 0)
                {
                    gCurrentScene->initialise(); // Reload the current level
                }
                else
                {
                    PlaySound(gLoseSound);     // Play the lose sound effect
                    switchToScene(SCENE_LOSE); // Switch to the lose scene
                }
            }
        }
        gTimeAccumulator -= FIXED_TIMESTEP;
    }
}

/**
 * @brief Render all game objects and UI.
 */
void render()
{
    BeginDrawing();

    if (gCurrentScene->getState().player) // If it is a game level (with players)
    {
        // for Camera
        Vector2 playerPos = {gCurrentScene->getState().player->getPosition().x, ORIGIN.y};
        Vector2 diff = Vector2Subtract(playerPos, gCurrentScene->getState().camera.target);
        gCurrentScene->getState().camera.target = Vector2Add(gCurrentScene->getState().camera.target, Vector2Scale(diff, 0.1f));

        BeginMode2D(gCurrentScene->getState().camera);
        gCurrentScene->render();
        EndMode2D();

        // render health lives
        DrawText(TextFormat("LIVES: %d", gLives), 20, 20, 30, WHITE);
    }
    else // a menu (no player)
    {
        gCurrentScene->render();
    }

    EndDrawing();
}

/**
 * @brief Clear and close the game.
 */
void shutdown()
{
    gMenuScene->shutdown();
    gLevelAScene->shutdown();
    gLevelBScene->shutdown();
    gLevelCScene->shutdown();
    gWinScene->shutdown();
    gLoseScene->shutdown();

    delete gMenuScene;
    delete gLevelAScene;
    delete gLevelBScene;
    delete gLevelCScene;
    delete gWinScene;
    delete gLoseScene;

    UnloadSound(gLoseSound);
    CloseAudioDevice();
    CloseWindow();
}

int main()
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }
    shutdown();
    return 0;
}