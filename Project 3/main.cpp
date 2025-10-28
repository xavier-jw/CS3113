/**
 * Author: [Junjie Wang]
 * Assignment: Project 3: Lunar Lander!
 * Date due: 2025-10-27, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/Entity.h"

// Game State Enumeration
enum GameState
{
    PLAYING,
    MISSION_FAILED,
    MISSION_ACCOMPLISHED
};

// Global constants
constexpr int SCREEN_WIDTH = 1000,
              SCREEN_HEIGHT = 600,
              FPS = 60;

constexpr char BG_COLOUR[] = "#1A1A2A"; // Deep Blue
constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};

// Game constants
constexpr float GRAVITY_ACCELERATION = 30.0f; // Simulated gravitational acceleration

constexpr float THRUST_POWER = 100.0f;         // Spacecraft thruster power
constexpr float FUEL_CONSUMPTION = 0.5f;       // Fuel consumed per key press
constexpr float MOVING_PLATFORM_SPEED = 70.0f; // The speed of moving platforms

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global variables
AppStatus gAppStatus = RUNNING;
GameState gGameState = PLAYING;
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;
float gFuel = 280.0f; // Player's initial fuel

Entity *gPlayerShip = nullptr; // Player's ship
Entity *gLandingPad = nullptr; // Victory Platform
Entity *gBomb = nullptr;       // Failed Platform

// Function declaration
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function definition

/**
 * @brief Initialise the game window, all entities (players, platforms, bombs) and game variables.
 */
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 3: Lunar Lander");

    // [Extra Credit] Defining ship's animation
    //  Use DOWN to denote the stationary state and UP to denote the move state.
    std::map<Direction, std::vector<int>> shipAnimationAtlas;
    // (DOWN)
    shipAnimationAtlas[DOWN] = {0};
    // (UP)
    shipAnimationAtlas[UP] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // 1.Initialise the ship
    gPlayerShip = new Entity(
        {ORIGIN.x, 80.0f},           // position
        {150.0f, 150.0f},            // scale
        "assets/ship_animation.png", // texture file address
        ATLAS,                       // single image or atlas?
        {10.0f, 1.0f},               // atlas dimensions
        shipAnimationAtlas           // actual atlas
    );

    // Set collision box
    gPlayerShip->setColliderDimensions({40.0f, 40.0f});
    // Apply initial gravity
    gPlayerShip->setAcceleration({0.0f, GRAVITY_ACCELERATION});
    gPlayerShip->setFrameSpeed(12);

    // 2. Initialise the Victory Platform
    gLandingPad = new Entity(
        {ORIGIN.x, SCREEN_HEIGHT - 80.0f}, // position
        {160.0f, 80.0f},                   // scale
        "assets/platform.png"              // texture file address
    );
    gLandingPad->setColliderDimensions({120.0f, 30.0f});
    gLandingPad->setVelocity({MOVING_PLATFORM_SPEED, 0.0f});

    // 3. Initialise the failed Platform
    gBomb = new Entity(
        {ORIGIN.x - 200.0f, ORIGIN.y + 100.0f}, // position
        {50.0f, 50.0f},                         // scale
        "assets/bomb.png"                       // texture file address
    );
    gBomb->setColliderDimensions({50.0f, 50.0f});
    gBomb->setVelocity({-MOVING_PLATFORM_SPEED * 0.6f, 0.0f});

    SetTargetFPS(FPS);
}

/**
 * @brief Process player input to control the ship movement
 */
void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose())
    {
        gAppStatus = TERMINATED;
        return;
    }

    if (gGameState != PLAYING)
        return;

    gPlayerShip->setAcceleration({0.0f, GRAVITY_ACCELERATION});

    // Used to track whether the W key has been pressed
    bool isThrusting = false;

    if (gFuel > 0)
    {
        if (IsKeyDown(KEY_A))
        {
            gPlayerShip->addAcceleration({-THRUST_POWER, 0.0f});
            gFuel -= FUEL_CONSUMPTION;
        }
        if (IsKeyDown(KEY_D))
        {
            gPlayerShip->addAcceleration({THRUST_POWER, 0.0f});
            gFuel -= FUEL_CONSUMPTION;
        }
        if (IsKeyDown(KEY_W))
        {

            gPlayerShip->addAcceleration({0.0f, -THRUST_POWER});
            gFuel -= FUEL_CONSUMPTION;
            isThrusting = true;
        }
    }

    // Extra Credit
    if (isThrusting)
    {
        gPlayerShip->setDirection(UP); // Play the animation
    }
    else
    {
        gPlayerShip->setDirection(DOWN);
    }

    if (gFuel < 0)
        gFuel = 0;
}

/**
 * @brief Update the game world, including physics, collision detection, and game state.
 */
void update()
{
    if (gAppStatus == TERMINATED)
        return;

    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        if (gGameState == PLAYING)
        {
            gPlayerShip->update(FIXED_TIMESTEP);
            gLandingPad->update(FIXED_TIMESTEP);
            gBomb->update(FIXED_TIMESTEP);

            // Check ship's boundaries
            Vector2 shipPos = gPlayerShip->getPosition();
            Vector2 shipCollider = gPlayerShip->getColliderDimensions();
            float leftEdge = shipPos.x - shipCollider.x / 2.0f;
            float rightEdge = shipPos.x + shipCollider.x / 2.0f;
            float topEdge = shipPos.y - shipCollider.y / 2.0f;
            // The ship must not cross the boundary.
            if (leftEdge < 0)
            {
                gPlayerShip->setPosition({shipCollider.x / 2.0f, shipPos.y});
                gPlayerShip->setVelocity({0.0f, gPlayerShip->getVelocity().y});
            }
            if (rightEdge > SCREEN_WIDTH)
            {
                gPlayerShip->setPosition({SCREEN_WIDTH - shipCollider.x / 2.0f, shipPos.y});
                gPlayerShip->setVelocity({0.0f, gPlayerShip->getVelocity().y});
            }
            if (topEdge < 0)
            {
                gPlayerShip->setPosition({shipPos.x, shipCollider.y / 2.0f});
                gPlayerShip->setVelocity({gPlayerShip->getVelocity().x, 0.0f});
            }

            // Update two platforms
            Vector2 padPos = gLandingPad->getPosition();
            if (padPos.x < 0 || padPos.x > SCREEN_WIDTH)
            {
                gLandingPad->setVelocity({-gLandingPad->getVelocity().x, 0.0f});
            }
            Vector2 bombPos = gBomb->getPosition();
            if (bombPos.x < 0 || bombPos.x > SCREEN_WIDTH)
            {
                gBomb->setVelocity({-gBomb->getVelocity().x, 0.0f});
            }

            // Collision detection and game state updates
            // Check whether it collided with the bomb (failed)
            if (gPlayerShip->isColliding(gBomb))
            {
                gGameState = MISSION_FAILED;
            }

            // Check whether it has collided with the bottom (failed)
            if (gPlayerShip->getPosition().y > SCREEN_HEIGHT - (gPlayerShip->getScale().y / 2.0f))
            {
                gGameState = MISSION_FAILED;
            }

            // Check whether it has collided with the landing platform (Victory)
            if (gPlayerShip->isColliding(gLandingPad))
            {
                gGameState = MISSION_ACCOMPLISHED;
            }
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    gTimeAccumulator = deltaTime;
}

/**
 * @brief Render all game objects and UI.
 */
void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    gPlayerShip->render();
    gLandingPad->render();
    gBomb->render();

    DrawText(TextFormat("FUEL: %d", (int)gFuel), 10, 10, 20, (gFuel > 0) ? WHITE : RED);

    DrawText(TextFormat("Horizontal-Speed: %.0f", gPlayerShip->getVelocity().x), SCREEN_WIDTH - 300, 10, 20, WHITE);

    DrawText(TextFormat("Vertical-Speed: %.0f", gPlayerShip->getVelocity().y), SCREEN_WIDTH - 300, 40, 20, WHITE);

    //  Display victory/defeat messages
    if (gGameState == MISSION_FAILED)
    {
        DrawText("MISSION FAILED", ORIGIN.x - MeasureText("MISSION FAILED", 40) / 2, ORIGIN.y - 20, 40, RED);
    }
    else if (gGameState == MISSION_ACCOMPLISHED)
    {
        DrawText("MISSION ACCOMPLISHED", ORIGIN.x - MeasureText("MISSION ACCOMPLISHED", 40) / 2, ORIGIN.y - 20, 40, GREEN);
    }

    EndDrawing();
}

/**
 * @brief Clear and close the game.
 */
void shutdown()
{
    // Release dynamically allocated memory
    delete gPlayerShip;
    delete gLandingPad;
    delete gBomb;

    CloseWindow();
}

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