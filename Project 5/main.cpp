#include "CS3113/cs3113.h"
#include "CS3113/MenuScreen.h"
#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/ShaderProgram.h"
#include "CS3113/Effects.h"

//  Screen dimension constants
constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 750;

// Frame rate and fixed timestep settings
constexpr int FPS = 60;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
constexpr Vector2 ORIGIN = {(float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2};

AppStatus gAppStatus = RUNNING;

// Time accumulator for fixed timestep updates
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;

// Scene pointer definitions
Scene *gCurrentScene = nullptr;
MenuScreen *gMenuScene = nullptr;
LevelA *gLevelAScene = nullptr;
LevelB *gLevelBScene = nullptr;
LevelC *gLevelCScene = nullptr;
MenuScreen *gLoseScene = nullptr;
MenuScreen *gWinScene = nullptr;

// Global game state variables (Lives, Cooldown, Sounds)
int gLives = 5;
int gPreviousLives = 5; // Used to detect damage taken
float gHitCooldown = 0.0f;
Sound gLoseSound;
Sound gVictorySound;

// Shader related variables (Hit red flash effect)
ShaderProgram gShader;
float gHurtFlashIntensity = 0.0f; // 0.0 normal, 1.0 full red
const float FLASH_DECAY_SPEED = 2.0f;

// Effects object pointer (Fade in)
Effects *gEffects = nullptr;

// Text constant definitions
const char *GAME_TITLE = "DEMON HUNTER";
const char *PRESS_ENTER = "Press Enter to Start";

const char *STORY_TEXT = "You are an adventurer commissioned to destroy the demon.\n\n"
                         "You will pass through\n"
                         "LevelA: Village\n"
                         "LevelB: Forest\n"
                         "LevelC: Boss stages.";

const char *CONTROL_TEXT = "Controls: WASD to Move, J to Attack";

/**
 * Function to switch scenes
 * @param id: The target scene ID
 */
void switchToScene(SceneID id)
{
    // Switch current scene pointer based on ID
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
    case SCENE_LOSE:
        gCurrentScene = gLoseScene;
        break;
    case SCENE_WIN:
        gCurrentScene = gWinScene;
        PlaySound(gVictorySound); // Play victory sound
        break;
    default:
        break;
    }

    if (gCurrentScene)
    {
        gCurrentScene->initialise();
        // If entering a game level (not menu), sync player health
        if (gCurrentScene->getState().player && id != SCENE_MENU)
        {
            if (gLives > 0)
            {
                gCurrentScene->getState().player->setHealth(gLives);
                gPreviousLives = gLives;
            }
        }
    }

    // Handle fade-in effects
    if (gEffects)
    {
        // Only trigger fade-in effect when entering Level A, B, or C
        if (id == SCENE_LEVEL_A || id == SCENE_LEVEL_B || id == SCENE_LEVEL_C)
        {
            gEffects->start(FADEIN);
        }
        else
        {
            // Menu scenes do not trigger fade-in, display instantly
        }
    }
}

/**
 * Initialisation function: Load resources, create objects
 */
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 5: Demon Hunter");
    InitAudioDevice();

    // Load sounds
    gLoseSound = LoadSound("assets/audio/lose.ogg");
    gVictorySound = LoadSound("assets/audio/victory.wav");

    // Load shaders
    gShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");

    // Create Effects object
    gEffects = new Effects(ORIGIN, (float)SCREEN_WIDTH * 1.5f, (float)SCREEN_HEIGHT * 1.5f);

    // Create all scene objects
    gMenuScene = new MenuScreen(GAME_TITLE, PRESS_ENTER, STORY_TEXT, CONTROL_TEXT, SCENE_LEVEL_A);
    gLevelAScene = new LevelA(ORIGIN, "#be8b80ff");
    gLevelBScene = new LevelB(ORIGIN, "#405c36");
    gLevelCScene = new LevelC(ORIGIN, "#405c36");
    gLoseScene = new MenuScreen("You Died...", "Press Enter to Restart", SCENE_MENU);
    gWinScene = new MenuScreen("VICTORY!", "Congratulations! Demon Defeated!\nPress Enter to Restart", SCENE_MENU);

    switchToScene(SCENE_MENU);
    SetTargetFPS(FPS);
}

/**
 * Input processing function
 */
void processInput()
{
    // Press Q or close window to quit
    if (IsKeyPressed(KEY_Q) || WindowShouldClose())
        gAppStatus = TERMINATED;

    // Handle player movement input (only when player is alive)
    if (gCurrentScene && gCurrentScene->getState().player)
    {
        Entity *player = gCurrentScene->getState().player;
        if (player->getAIState() != DYING && player->getAIState() != DEAD)
        {
            player->resetMovement();
            if (IsKeyDown(KEY_A))
                player->moveLeft();
            else if (IsKeyDown(KEY_D))
                player->moveRight();
            if (IsKeyDown(KEY_W))
                player->moveUp();
            else if (IsKeyDown(KEY_S))
                player->moveDown();
            if (Vector2Length(player->getMovement()) > 0)
                player->normaliseMovement();
        }
    }
}

/**
 * Logic update function
 */
void update()
{
    // Calculate Delta Time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gTimeAccumulator += deltaTime;

    // Fixed timestep loop
    while (gTimeAccumulator >= FIXED_TIMESTEP)
    {
        // Update current scene
        gCurrentScene->update(FIXED_TIMESTEP);
        GameState &state = gCurrentScene->getState();

        // Update effects position
        Vector2 viewPos = ORIGIN;
        if (state.player)
        {
            viewPos = state.player->getPosition();
        }
        gEffects->update(FIXED_TIMESTEP, &viewPos);

        if (gHitCooldown > 0.0f)
        {
            gHitCooldown -= FIXED_TIMESTEP;
        }

        // Check for scene switch requests
        if (state.nextSceneID != -1)
        {
            // If returning to menu, reset global state
            if (state.nextSceneID == SCENE_MENU)
            {
                gLives = 5;
                gPreviousLives = 5;
                gHitCooldown = 0.0f;
            }
            switchToScene((SceneID)state.nextSceneID);
        }

        // Player state logic
        if (state.player)
        {
            gLives = state.player->getHealth();

            // Hit detection: If health decreases, trigger red flash
            if (gLives < gPreviousLives)
            {
                gHurtFlashIntensity = 0.8f;
            }
            gPreviousLives = gLives;

            // Death detection: Play lose sound and switch scene
            if (state.player->getAIState() == DEAD)
            {
                PlaySound(gLoseSound);
                switchToScene(SCENE_LOSE);
                gTimeAccumulator -= FIXED_TIMESTEP;
                continue;
            }

            // Set dying state
            if (gLives <= 0 && state.player->getAIState() != DYING && state.player->getAIState() != DEAD)
            {
                state.player->setAIState(DYING);
            }
        }

        // Decay shader red effect
        if (gHurtFlashIntensity > 0.0f)
        {
            gHurtFlashIntensity -= FLASH_DECAY_SPEED * FIXED_TIMESTEP;
            if (gHurtFlashIntensity < 0.0f)
                gHurtFlashIntensity = 0.0f;
        }

        gTimeAccumulator -= FIXED_TIMESTEP;
    }
}

/**
 * Render function
 */
void render()
{
    BeginDrawing();

    // If current scene has a player (is a game level)
    if (gCurrentScene->getState().player)
    {
        BeginMode2D(gCurrentScene->getState().camera);

        gShader.begin();
        gShader.setFloat("flashFactor", gHurtFlashIntensity);

        // Render scene (affected by Shader)
        gCurrentScene->render();

        gShader.end();

        // EN: Render effects overlay (after Shader, inside camera mode)
        gEffects->render();

        EndMode2D();

        // Draw Lives UI
        if (gLives > 0)
        {
            DrawText(TextFormat("LIVES: %d", gLives), 20, 20, 30, WHITE);
        }
        else
        {
            DrawText("LIVES: 0", 20, 20, 30, RED);
        }
    }
    else
    {
        // Render menu scene
        gCurrentScene->render();
        gEffects->render();
    }
    EndDrawing();
}

/**
 * Shutdown function: Cleanup resources
 */
void shutdown()
{
    gMenuScene->shutdown();
    gLevelAScene->shutdown();
    gLevelBScene->shutdown();
    gLevelCScene->shutdown();
    gLoseScene->shutdown();
    gWinScene->shutdown();

    gShader.unload();

    delete gEffects;

    delete gMenuScene;
    delete gLevelAScene;
    delete gLevelBScene;
    delete gLevelCScene;
    delete gLoseScene;
    delete gWinScene;

    UnloadSound(gLoseSound);
    UnloadSound(gVictorySound);
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