/**
 * Author: [Junjie Wang]
 * Assignment: Simple 2D Scene
 * Date due: 2025-09-27, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/cs3113.h"
#include <math.h>

// Global Constants
constexpr int SCREEN_WIDTH = 1600,
              SCREEN_HEIGHT = 900,
              FPS = 60;

// Texture path
// Image owned by Kazuki Takahashi
constexpr char TEXTURE1_PATH[] = "assets/1.jpg";
constexpr char TEXTURE2_PATH[] = "assets/2.jpg";
constexpr char TEXTURE3_PATH[] = "assets/3.jpg";

constexpr int BG_COLOUR_CHANGE_FRAMES = 120; // Background Color changes every 120 frames
// Background colours set
const char *gBgColours[] = {
    "#B2AAC6",
    "#AFEEEE",
    "#FFDAB9"};

constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};

struct TextureObject
{
    Texture2D texture;
    Vector2 position;
    Vector2 scale;

    // Movement attributes
    float rotation;
    float rotationSpeed;
    float pulseTime;
    float orbitAngle;
    float orbitRadius;
    float orbitSpeed;
};

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;

// Used to change BgColour
int gFrameCounter = 0;
int gCurrentBgColourIndex = 0;

TextureObject texture1;
TextureObject texture2;
TextureObject texture3;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Helper function to draw texture
void DrawTexture(const TextureObject &obj);

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 1");
    SetTargetFPS(FPS);

    // load texture
    texture1.texture = LoadTexture(TEXTURE1_PATH);
    texture2.texture = LoadTexture(TEXTURE2_PATH);
    texture3.texture = LoadTexture(TEXTURE3_PATH);

    // initialize texture1
    texture1.position = ORIGIN;
    texture1.scale = {0.75f, 0.75f};
    texture1.rotation = 0.0f;
    texture1.pulseTime = 0.0f;
    texture1.rotationSpeed = 20.0f;

    texture2.scale = {0.5f, 0.5f};
    texture2.rotation = 0.0f;
    texture2.orbitAngle = 0.0f;
    texture2.orbitRadius = 200.0f;
    texture2.orbitSpeed = 0.5f;
    texture2.rotationSpeed = 0.0f;

    texture3.scale = {0.25f, 0.25f};
    texture3.rotation = 0.0f;
    texture3.orbitAngle = 0.0f;
    texture3.orbitRadius = 75.0f;
    texture3.orbitSpeed = 1.5f;
    texture3.rotationSpeed = 50.0f;
}

void processInput()
{
    if (WindowShouldClose())
        gAppStatus = TERMINATED;
}

void update()
{
    // Delta time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // Background Color Switching Logic
    gFrameCounter++;
    if (gFrameCounter >= BG_COLOUR_CHANGE_FRAMES)
    {
        gFrameCounter = 0;
        // Cycle through color index(0, 1, 2, 0, 1, 2)
        gCurrentBgColourIndex = (gCurrentBgColourIndex + 1) % 3;
    }

    // 1. Update "textrue1" (Pulse effect)
    texture1.pulseTime += 1.0f * deltaTime;
    float pulseFactor = 0.1f * sin(texture1.pulseTime * 2.0f);
    texture1.scale = {1.0f + pulseFactor, 1.0f + pulseFactor};
    texture1.rotation += texture1.rotationSpeed * deltaTime; // Update rotation angle

    // 2. Update "textrue2"
    texture2.orbitAngle += texture2.orbitSpeed * deltaTime;
    texture2.position.x = texture1.position.x + texture2.orbitRadius * cos(texture2.orbitAngle);
    texture2.position.y = texture1.position.y + texture2.orbitRadius * sin(texture2.orbitAngle);

    // 3. Update "textrue3"
    texture3.orbitAngle += texture3.orbitSpeed * deltaTime;
    texture3.rotation += texture3.rotationSpeed * deltaTime; // Update rotation angle
    texture3.position.x = texture2.position.x + texture3.orbitRadius * cos(texture3.orbitAngle);
    texture3.position.y = texture2.position.y + texture3.orbitRadius * sin(texture3.orbitAngle);
}

// Helper function to draw texture
void DrawTexture(const TextureObject &obj)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {0.0f, 0.0f, (float)obj.texture.width, (float)obj.texture.height};

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        obj.position.x,
        obj.position.y,
        // Define the size of the texture
        (float)obj.texture.width * obj.scale.x,
        (float)obj.texture.height * obj.scale.y};

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin = {destinationArea.width / 2.0f, destinationArea.height / 2.0f};
    DrawTexturePro(
        obj.texture,
        textureArea,
        destinationArea,
        objectOrigin,
        obj.rotation,
        WHITE);
}

void render()
{
    BeginDrawing();
    // choose Background colour
    ClearBackground(ColorFromHex(gBgColours[gCurrentBgColourIndex]));

    DrawTexture(texture1);
    DrawTexture(texture2);
    DrawTexture(texture3);

    EndDrawing();
}

void shutdown() { CloseWindow(); }

int main(void)
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