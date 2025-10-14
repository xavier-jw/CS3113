/**
 * Author: [Junjie Wang]
 * Assignment: Pong Clone
 * Date due: 2025-10-13, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/cs3113.h"
#include <math.h>

// Global Constants
constexpr int SCREEN_WIDTH = 1600 / 2,
              SCREEN_HEIGHT = 900 / 2,
              FPS = 60;

constexpr char BG_COLOUR[] = "#F8F1C8";

// Supports up to 3 balls
constexpr int MAX_BALLS = 3;

// Game Object attributes
constexpr float PADDLE_SPEED = 350.0f;
constexpr float BALL_SPEED = 150.0f;
constexpr Vector2 PADDLE_SIZE = {90.0f, 100.0f};
constexpr Vector2 BALL_SIZE = {25.0f, 25.0f};

constexpr char PADDLE_FP[] = "assets/paddle.png";
constexpr char BALL_FP[] = "assets/ball.png";
constexpr char PLAYER1_WINS_FP[] = "assets/player1.png";
constexpr char PLAYER2_WINS_FP[] = "assets/player2.png";

struct Paddle
{
    Vector2 position;
    Vector2 scale;
    Vector2 movement;
    Texture2D texture;
};

struct Ball
{
    Vector2 position;
    Vector2 scale;
    Vector2 velocity;
    Texture2D texture;
};

// Global Variables
AppStatus gAppStatus = RUNNING;
bool gIsGameOver = false;
bool gIsSinglePlayer = false;

int gActiveBalls = 1;
float gPreviousTicks = 0.0f;
Paddle gPlayer1;
Paddle gPlayer2;
Ball gBalls[MAX_BALLS]; // array, used to hold all possible balls
Texture2D gPaddleTexture;
Texture2D gBallTexture;

// for extra credit
int gWinner = 0; // 0 = null, 1 = P1 wins, 2 = P2 wins
Texture2D gP1WinsTexture;
Texture2D gP2WinsTexture;

// Function Declarations
void initialise();
void resetGame();
void processInput();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);
void update();
void renderObject(const Texture2D *texture, const Vector2 *position, const Vector2 *scale);
void render();
void shutdown();

// Function Definitions

/**
 * @brief Set the initial state of the game.
 */
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 2: Pong");
    SetTargetFPS(FPS);

    // load Texture
    gPaddleTexture = LoadTexture(PADDLE_FP);
    gBallTexture = LoadTexture(BALL_FP);
    gP1WinsTexture = LoadTexture(PLAYER1_WINS_FP);
    gP2WinsTexture = LoadTexture(PLAYER2_WINS_FP);

    // Initialize Player1, Play2 and balls
    resetGame();
}

/**
 * @brief Helper function to initialize the game
 */
void resetGame()
{
    // Initialize Player 1 (Left)
    gPlayer1.position = {PADDLE_SIZE.x, SCREEN_HEIGHT / 2.0f};
    gPlayer1.scale = PADDLE_SIZE;
    gPlayer1.movement = {0.0f, 0.0f};
    gPlayer1.texture = gPaddleTexture;

    // Initialize Player 2 (right side)
    gPlayer2.position = {SCREEN_WIDTH - PADDLE_SIZE.x, SCREEN_HEIGHT / 2.0f};
    gPlayer2.scale = PADDLE_SIZE;
    gPlayer2.movement = {0.0f, 0.0f};
    gPlayer2.texture = gPaddleTexture;

    const Vector2 initialVelocities[MAX_BALLS] = {
        {0.8f, -0.8f},
        {-0.7f, -0.7f},
        {0.6f, -0.6f}};

    // Initialize balls
    for (int i = 0; i < MAX_BALLS; ++i)
    {
        gBalls[i].position = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        gBalls[i].scale = BALL_SIZE;
        gBalls[i].texture = gBallTexture;

        // Set each ball a different initial direction.
        gBalls[i].velocity = initialVelocities[i];
    }
}

/**
 * @brief Process all user input.
 */
void processInput()
{
    if (WindowShouldClose())
    {
        gAppStatus = TERMINATED;
    }

    // While the game is running
    if (IsKeyPressed(KEY_T))
        gIsSinglePlayer = !gIsSinglePlayer;
    if (IsKeyPressed(KEY_ONE))
        gActiveBalls = 1;
    if (IsKeyPressed(KEY_TWO))
        gActiveBalls = 2;
    if (IsKeyPressed(KEY_THREE))
        gActiveBalls = 3;

    // Used to control play1's movement
    gPlayer1.movement = {0.0f, 0.0f};
    if (IsKeyDown(KEY_W))
        gPlayer1.movement.y = -1;
    if (IsKeyDown(KEY_S))
        gPlayer1.movement.y = 1;

    gPlayer2.movement = {0.0f, 0.0f};
    if (!gIsSinglePlayer)
    {
        if (IsKeyDown(KEY_UP))
            gPlayer2.movement.y = -1;
        if (IsKeyDown(KEY_DOWN))
            gPlayer2.movement.y = 1;
    }
}

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 *
 * @see
 *
 * @param positionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);
    return (xDistance < 0.0f && yDistance < 0.0f);
}

void update()
{
    if (gIsGameOver || gAppStatus != RUNNING)
        return;

    // Use Delta time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // SinglePlayer mode
    if (gIsSinglePlayer)
    {
        // follow first ball's movement
        if (gPlayer2.position.y < gBalls[0].position.y)
            gPlayer2.movement.y = 1.0f;
        else if (gPlayer2.position.y > gBalls[0].position.y)
            gPlayer2.movement.y = -1.0f;
    }

    gPlayer1.position.y += gPlayer1.movement.y * PADDLE_SPEED * deltaTime;
    gPlayer2.position.y += gPlayer2.movement.y * PADDLE_SPEED * deltaTime;

    float paddleHalfHeight = PADDLE_SIZE.y / 2.0f;
    // Prevent two paddles from crossing boundaries
    if (gPlayer1.position.y < paddleHalfHeight)
        gPlayer1.position.y = paddleHalfHeight;
    if (gPlayer1.position.y > SCREEN_HEIGHT - paddleHalfHeight)
        gPlayer1.position.y = SCREEN_HEIGHT - paddleHalfHeight;
    if (gPlayer2.position.y < paddleHalfHeight)
        gPlayer2.position.y = paddleHalfHeight;
    if (gPlayer2.position.y > SCREEN_HEIGHT - paddleHalfHeight)
        gPlayer2.position.y = SCREEN_HEIGHT - paddleHalfHeight;

    // By HINT: Generate every object that could be on the screen at any given time,
    // but any object that isn't currently being used should not be rendered nor updated.
    for (int i = 0; i < gActiveBalls; ++i)
    {
        // Implement ball movement
        gBalls[i].position.x += gBalls[i].velocity.x * BALL_SPEED * deltaTime;
        gBalls[i].position.y += gBalls[i].velocity.y * BALL_SPEED * deltaTime;

        float ballHalfHeight = BALL_SIZE.y / 2.0f;
        // if balls Touch Top and bottom of the screen
        if (gBalls[i].position.y < ballHalfHeight || gBalls[i].position.y > SCREEN_HEIGHT - ballHalfHeight)
        {
            gBalls[i].velocity.y *= -1;
        }
        // if balls touch paddles
        if (isColliding(&gBalls[i].position, &gBalls[i].scale, &gPlayer1.position, &gPlayer1.scale) ||
            isColliding(&gBalls[i].position, &gBalls[i].scale, &gPlayer2.position, &gPlayer2.scale))
        {
            gBalls[i].velocity.x *= -1;
        }

        float ballHalfWidth = BALL_SIZE.x / 2.0f;
        // Determine the winning condition
        if (gBalls[i].position.x < ballHalfWidth)
        {
            gIsGameOver = true;
            gWinner = 2; // Ball went out of bound on the left side. Player 2 wins.
        }
        else if (gBalls[i].position.x > SCREEN_WIDTH - ballHalfWidth)
        {
            gIsGameOver = true;
            gWinner = 1; // The ball went out of bounds on the right side. Player 1 wins.
        }
    }
}

/**
 * @brief Helper function to render a single textured object to the screen.
 */
void renderObject(const Texture2D *texture, const Vector2 *position, const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height)};
    // Destination rectangle – centred on gPosition
    Rectangle destArea = {position->x, position->y, scale->x, scale->y};
    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {scale->x / 2.0f, scale->y / 2.0f};
    DrawTexturePro(*texture, textureArea, destArea, originOffset, 0.0f, WHITE);
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    // If the game ends, display the ending screen
    if (gIsGameOver)
    {
        if (gWinner == 1)
        {
            // Show Player 1 wins
            DrawTexture(gP1WinsTexture, SCREEN_WIDTH / 2 - gP1WinsTexture.width / 2, SCREEN_HEIGHT / 2 - gP1WinsTexture.height / 2, WHITE);
        }
        else if (gWinner == 2)
        {
            // Show Player 2 wins
            DrawTexture(gP2WinsTexture, SCREEN_WIDTH / 2 - gP2WinsTexture.width / 2, SCREEN_HEIGHT / 2 - gP2WinsTexture.height / 2, WHITE);
        }
    }
    else
    {
        renderObject(&gPlayer1.texture, &gPlayer1.position, &gPlayer1.scale);
        renderObject(&gPlayer2.texture, &gPlayer2.position, &gPlayer2.scale);
        // Generate the player-specified number of balls
        for (int i = 0; i < gActiveBalls; ++i)
        {
            renderObject(&gBalls[i].texture, &gBalls[i].position, &gBalls[i].scale);
        }
    }
    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gPaddleTexture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gP1WinsTexture);
    UnloadTexture(gP2WinsTexture);
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