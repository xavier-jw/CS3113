#include "LevelA.h"
#include "Windmill.h"
#include "raymath.h"

// Map size: 24 columns x 16 rows (50 pixels per cell => 1200 x 800 total)
constexpr int LEVEL_WIDTH_A = 24;
constexpr int LEVEL_HEIGHT_A = 16;

constexpr unsigned int LEVEL_DATA_A[] = {
    2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 1, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 1, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2};

void LevelA::initialise()
{
   // Clear old data
   for (Entity *enemy : mGameState.enemies)
      delete enemy;
   mGameState.enemies.clear();
   for (Entity *prop : mGameState.scenery)
      delete prop;
   mGameState.scenery.clear();
   if (mGameState.player)
   {
      delete mGameState.player;
      mGameState.player = nullptr;
   }
   if (mGameState.map)
   {
      delete mGameState.map;
      mGameState.map = nullptr;
   }

   mGameState.nextSceneID = -1;

   // 1. Load map
   mGameState.map = new Map(LEVEL_WIDTH_A, LEVEL_HEIGHT_A, (unsigned int *)LEVEL_DATA_A, "assets/game/tileset_1.png", 50.0f, 3, 1, mOrigin);

   // 2. Load texture
   mTexPlayerWalk = LoadTexture("assets/game/character_walk.png");
   mTexPlayerAttack = LoadTexture("assets/game/character_attack.png");
   mTexPlayerDeath = LoadTexture("assets/game/character_death.png");
   mTexSoldierWalk = LoadTexture("assets/game/Soldier_walk.png");
   mTexSoldierAttack = LoadTexture("assets/game/Soldier_attack.png");
   mTexSoldierDeath = LoadTexture("assets/game/Soldier_death.png");

   // Audio
   mGameState.bgm = LoadMusicStream("assets/audio/levelA_background.mp3");
   SetMusicVolume(mGameState.bgm, 0.5f);
   PlayMusicStream(mGameState.bgm);

   mGameState.attackSound = LoadSound("assets/audio/attack.wav");
   mGameState.hurtSound = LoadSound("assets/audio/hurt.wav");

   // 3. Define the animation
   // Walk: 6 Columns (0-5, 6-11, ...)
   mWalkAnim = {
       {DOWN, {0, 1, 2, 3, 4, 5}},
       {LEFT, {6, 7, 8, 9, 10, 11}},
       {RIGHT, {12, 13, 14, 15, 16, 17}},
       {UP, {18, 19, 20, 21, 22, 23}}};

   // Attack: 6 Columns
   mAttackAnim = mWalkAnim;

   // Row 0 (Down): 0-6
   // Row 1 (Left): 7-13
   // Row 2 (Right): 14-20
   // Row 3 (Up): 21-27
   mDeathAnim = {
       {DOWN, {0, 1, 2, 3, 4, 5, 6}},
       {LEFT, {7, 8, 9, 10, 11, 12, 13}},
       {RIGHT, {14, 15, 16, 17, 18, 19, 20}},
       {UP, {21, 22, 23, 24, 25, 26, 27}}};

   // 4. Create Player
   mGameState.player = new Entity({1000.0f, 400.0f}, {40.0f, 40.0f}, "assets/game/character_walk.png", ATLAS, {6, 4}, mWalkAnim, PLAYER);
   mGameState.player->setSpeed(150.0f);
   mGameState.player->setColliderDimensions({20.0f, 20.0f});

   mGameState.player->setHealth(5);

   // 5. Place Scenery (Houses/Windmill)

   // House 1
   Entity *house1 = new Entity({150.0f, 150.0f}, {100.0f, 100.0f}, "assets/game/house_1.png", PROP);
   house1->setColliderDimensions({80.0f, 60.0f});
   mGameState.scenery.push_back(house1);

   // House 2: Windmill house
   Vector2 house2Pos = {550.0f, 200.0f};
   Entity *house2Base = new Entity(house2Pos, {55.0f, 120.0f}, "assets/game/house_2.png", PROP);
   house2Base->setColliderDimensions({55.0f, 100.0f});
   mGameState.scenery.push_back(house2Base);

   // Add rotating windmill blades
   Windmill *windmill = new Windmill({house2Pos.x, house2Pos.y - 25.0f});
   mGameState.scenery.push_back(windmill);

   // House 3
   Entity *house3 = new Entity({450.0f, 600.0f}, {80.0f, 80.0f}, "assets/game/house_3.png", PROP);
   house3->setColliderDimensions({60.0f, 60.0f});
   mGameState.scenery.push_back(house3);

   // House 4
   Entity *house4 = new Entity({150.0f, 275.0f}, {90.0f, 90.0f}, "assets/game/house_4.png", PROP);
   house4->setColliderDimensions({70.0f, 70.0f});
   mGameState.scenery.push_back(house4);

   // Create Soldier Enemies
   Vector2 spawnPoints[] = {{375.0f, 200.0f}, {675.0f, 600.0f}, {1000.0f, 100.0f}, {1000.0f, 600.0f}};
   for (int i = 0; i < 4; i++)
   {
      Entity *soldier = new Entity(spawnPoints[i], {40.0f, 40.0f}, "assets/game/Soldier_walk.png", ATLAS, {6, 4}, mWalkAnim, ENEMY);
      soldier->setAIType(SOLDIER);
      soldier->setSpeed(30.0f);
      soldier->setColliderDimensions({20.0f, 20.0f});
      soldier->setHealth(6);

      soldier->setAttackPower(2); // Damage

      soldier->setTexture(mTexSoldierWalk, 6, 4, mWalkAnim);
      mGameState.enemies.push_back(soldier);
   }

   // Set camera to follow playe
   mGameState.camera.target = mGameState.player->getPosition();
   mGameState.camera.offset = mOrigin;
   mGameState.camera.zoom = 2.0f;
}

void LevelA::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   Entity *player = mGameState.player;
   if (player->getAIState() == DEAD)
      return;

   // Player dying state update
   if (player->getAIState() == DYING)
   {
      // Death texture has 7 columns
      player->setTexture(mTexPlayerDeath, 7, 4, mDeathAnim);
      player->update(deltaTime, player, mGameState.map, &mGameState.scenery);
      return;
   }

   // ------ Player Logic -------
   if (IsKeyPressed(KEY_J) && player->getAIState() != ATTACKING)
   {
      player->setAIState(ATTACKING);
      player->setTexture(mTexPlayerAttack, 6, 4, mAttackAnim);
      player->setHasDealtDamage(false);
      PlaySound(mGameState.attackSound);
   }

   // Handle player attack collision
   if (player->getAIState() == ATTACKING)
   {
      int currentFrame = player->getCurrentFrameIndex();
      // Deal damage at specific frame (frame 3)
      if (currentFrame == 3 && !player->hasDealtDamage())
      {
         Rectangle attackBox = player->getAttackBox();
         for (Entity *enemy : mGameState.enemies)
         {
            if (enemy->getAIState() != DEAD && enemy->getAIState() != DYING)
            {
               Rectangle enemyHurtBox = {
                   enemy->getPosition().x - enemy->getColliderDimensions().x / 2.0f,
                   enemy->getPosition().y - enemy->getColliderDimensions().y / 2.0f,
                   enemy->getColliderDimensions().x, enemy->getColliderDimensions().y};
               if (CheckCollisionRecs(attackBox, enemyHurtBox))
               {
                  enemy->takeDamage(player->getAttackPower());
                  // Calculate knockback effect
                  Vector2 knockback = Vector2Subtract(enemy->getPosition(), player->getPosition());
                  knockback = Vector2Normalize(knockback);
                  knockback = Vector2Scale(knockback, 20.0f);
                  enemy->setPosition(Vector2Add(enemy->getPosition(), knockback));
               }
            }
         }
         player->setHasDealtDamage(true);
      }
   }

   if (player->getAIState() == IDLE)
      player->setTexture(mTexPlayerWalk, 6, 4, mWalkAnim);

   // Update player physics (Includes collision with enemies and scenery
   std::vector<Entity *> playerObstacles = mGameState.scenery;
   for (Entity *enemy : mGameState.enemies)
   {
      if (enemy->getAIState() != DEAD && enemy->getAIState() != DYING)
         playerObstacles.push_back(enemy);
   }
   player->update(deltaTime, player, mGameState.map, &playerObstacles);

   // Scene transition logic
   float mapPixelWidth = LEVEL_WIDTH_A * 50.0f;
   float mapLeftEdge = mOrigin.x - mapPixelWidth / 2.0f;
   // If player walks to the far left edge, switch to Level B
   if (player->getPosition().x - player->getColliderDimensions().x / 2.0f <= mapLeftEdge + 10.0f)
   {
      mGameState.nextSceneID = SCENE_LEVEL_B;
   }

   // Player map boundary constraints
   float mapPixelHeight = LEVEL_HEIGHT_A * 50.0f;
   Vector2 pos = player->getPosition();
   Vector2 col = player->getColliderDimensions();
   if (pos.x - col.x / 2 < mOrigin.x - mapPixelWidth / 2)
      pos.x = mOrigin.x - mapPixelWidth / 2 + col.x / 2;
   if (pos.x + col.x / 2 > mOrigin.x + mapPixelWidth / 2)
      pos.x = mOrigin.x + mapPixelWidth / 2 - col.x / 2;
   if (pos.y - col.y / 2 < mOrigin.y - mapPixelHeight / 2)
      pos.y = mOrigin.y - mapPixelHeight / 2 + col.y / 2;
   if (pos.y + col.y / 2 > mOrigin.y + mapPixelHeight / 2)
      pos.y = mOrigin.y + mapPixelHeight / 2 - col.y / 2;
   player->setPosition(pos);

   // Update Scenery
   for (Entity *prop : mGameState.scenery)
   {
      prop->update(deltaTime, nullptr, mGameState.map, nullptr);
   }

   // Update Enemies
   std::vector<Entity *> enemyObstacles = mGameState.scenery;
   enemyObstacles.push_back(player); // Enemies view player as obstacle
   for (Entity *otherEnemy : mGameState.enemies)
   {
      if (otherEnemy->getAIState() != DEAD)
         enemyObstacles.push_back(otherEnemy);
   }

   for (Entity *enemy : mGameState.enemies)
   {
      enemy->update(deltaTime, player, mGameState.map, &enemyObstacles);

      Vector2 ePos = enemy->getPosition();
      Vector2 eCol = enemy->getColliderDimensions();
      if (ePos.x - eCol.x / 2 < mOrigin.x - mapPixelWidth / 2)
         ePos.x = mOrigin.x - mapPixelWidth / 2 + eCol.x / 2;
      if (ePos.x + eCol.x / 2 > mOrigin.x + mapPixelWidth / 2)
         ePos.x = mOrigin.x + mapPixelWidth / 2 - eCol.x / 2;
      if (ePos.y - eCol.y / 2 < mOrigin.y - mapPixelHeight / 2)
         ePos.y = mOrigin.y - mapPixelHeight / 2 + eCol.y / 2;
      if (ePos.y + eCol.y / 2 > mOrigin.y + mapPixelHeight / 2)
         ePos.y = mOrigin.y + mapPixelHeight / 2 - eCol.y / 2;
      enemy->setPosition(ePos);

      if (enemy->getAIState() == DYING)
         enemy->setTexture(mTexSoldierDeath, 7, 4, mDeathAnim); // 7 cols
      else if (enemy->getAIState() == ATTACKING)
         enemy->setTexture(mTexSoldierAttack, 6, 4, mAttackAnim); // 6 cols
      else
         enemy->setTexture(mTexSoldierWalk, 6, 4, mWalkAnim); // 6 cols

      // Enemy attack collision logic
      if (enemy->getAIState() == ATTACKING)
      {
         if (enemy->getCurrentFrameIndex() == 4 && !enemy->hasDealtDamage())
         {
            Rectangle playerHurtBox = {
                player->getPosition().x - player->getColliderDimensions().x / 2.0f,
                player->getPosition().y - player->getColliderDimensions().y / 2.0f,
                player->getColliderDimensions().x, player->getColliderDimensions().y};

            Rectangle enemyAttackBox = enemy->getAttackBox();
            if (CheckCollisionRecs(enemyAttackBox, playerHurtBox))
            {
               if (player->getAIState() != DEAD && player->getAIState() != DYING)
               {
                  player->takeDamage(enemy->getAttackPower());
                  Vector2 knockback = Vector2Subtract(player->getPosition(), enemy->getPosition());
                  knockback = Vector2Normalize(knockback);
                  knockback = Vector2Scale(knockback, 30.0f);
                  player->setPosition(Vector2Add(player->getPosition(), knockback));
                  PlaySound(mGameState.hurtSound);
               }
            }
            enemy->setHasDealtDamage(true);
         }
      }
   }

   // Camera tracking
   float zoom = mGameState.camera.zoom;
   float screenWidth = (float)GetScreenWidth() / zoom;
   float screenHeight = (float)GetScreenHeight() / zoom;
   Vector2 cameraTarget = player->getPosition();

   // Calculate min/max camera coordinates
   float minTargetX = mOrigin.x - mapPixelWidth / 2 + screenWidth / 2;
   float maxTargetX = mOrigin.x + mapPixelWidth / 2 - screenWidth / 2;
   float minTargetY = mOrigin.y - mapPixelHeight / 2 + screenHeight / 2;
   float maxTargetY = mOrigin.y + mapPixelHeight / 2 - screenHeight / 2;

   cameraTarget.x = fmaxf(minTargetX, fminf(cameraTarget.x, maxTargetX));
   cameraTarget.y = fmaxf(minTargetY, fminf(cameraTarget.y, maxTargetY));

   mGameState.camera.target = cameraTarget;
}

void LevelA::render()
{
   ClearBackground(ColorFromHex("#5c9345"));
   mGameState.map->render();
   // Render static scenery
   for (Entity *obj : mGameState.scenery)
      obj->render();
   for (Entity *enemy : mGameState.enemies)
   {
      if (enemy->getAIState() == DEAD || enemy->getAIState() == DYING)
         enemy->render();
   }
   // Render living enemies
   for (Entity *enemy : mGameState.enemies)
   {
      if (enemy->getAIState() != DEAD && enemy->getAIState() != DYING)
         enemy->render();
   }
   mGameState.player->render();
}

void LevelA::shutdown()
{
   delete mGameState.player;
   delete mGameState.map;
   for (Entity *obj : mGameState.scenery)
      delete obj;
   mGameState.scenery.clear();
   for (Entity *enemy : mGameState.enemies)
      delete enemy;
   mGameState.enemies.clear();

   UnloadTexture(mTexPlayerWalk);
   UnloadTexture(mTexPlayerAttack);
   UnloadTexture(mTexPlayerDeath);
   UnloadTexture(mTexSoldierWalk);
   UnloadTexture(mTexSoldierAttack);
   UnloadTexture(mTexSoldierDeath);

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.attackSound);
   UnloadSound(mGameState.hurtSound);
}