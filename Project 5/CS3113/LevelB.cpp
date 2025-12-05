#include "LevelB.h"
#include "raymath.h"

// Map size: 24 columns x 16 rows (50 pixels per cell => 1200 x 800 total)
constexpr int LEVEL_WIDTH_B = 24;
constexpr int LEVEL_HEIGHT_B = 16;

// 1 = Path
// 2 = Grass
constexpr unsigned int LEVEL_DATA_B[] = {
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

void LevelB::initialise()
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

   mGameState.map = new Map(LEVEL_WIDTH_B, LEVEL_HEIGHT_B, (unsigned int *)LEVEL_DATA_B,
                            "assets/game/tileset_2.png", 50.0f, 2, 1, mOrigin);

   mTexPlayerWalk = LoadTexture("assets/game/character_walk.png");
   mTexPlayerAttack = LoadTexture("assets/game/character_attack.png");
   mTexPlayerDeath = LoadTexture("assets/game/character_death.png");

   // Slime Textures
   mTexSlimeWalk = LoadTexture("assets/game/slime_walk.png");
   mTexSlimeAttack = LoadTexture("assets/game/slime_attack.png");
   mTexSlimeDeath = LoadTexture("assets/game/slime_death.png");

   mTexTree1 = LoadTexture("assets/game/tree_1.png");
   mTexTree2 = LoadTexture("assets/game/tree_2.png");
   mTexTree3 = LoadTexture("assets/game/tree_3.png");
   mTexTree4 = LoadTexture("assets/game/tree_4.png");

   // Load Audio
   mGameState.bgm = LoadMusicStream("assets/audio/levelB_background.mp3");
   SetMusicVolume(mGameState.bgm, 0.5f);
   PlayMusicStream(mGameState.bgm);
   mGameState.attackSound = LoadSound("assets/audio/attack.wav");
   mGameState.hurtSound = LoadSound("assets/audio/hurt.wav");

   // Define Animations
   mWalkAnim = {{DOWN, {0, 1, 2, 3, 4, 5}}, {LEFT, {6, 7, 8, 9, 10, 11}}, {RIGHT, {12, 13, 14, 15, 16, 17}}, {UP, {18, 19, 20, 21, 22, 23}}};
   mAttackAnim = mWalkAnim;
   mDeathAnim = mWalkAnim;

   // Slime Walk: 8 columns 4 rows
   mSlimeWalkAnim = {
       {DOWN, {0, 1, 2, 3, 4, 5, 6, 7}},
       {UP, {8, 9, 10, 11, 12, 13, 14, 15}},
       {LEFT, {16, 17, 18, 19, 20, 21, 22, 23}},
       {RIGHT, {24, 25, 26, 27, 28, 29, 30, 31}}};

   // Slime Attack: 10 columns 4 rows
   mSlimeAttackAnim = {
       {DOWN, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}},
       {UP, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19}},
       {LEFT, {20, 21, 22, 23, 24, 25, 26, 27, 28, 29}},
       {RIGHT, {30, 31, 32, 33, 34, 35, 36, 37, 38, 39}}};

   // Slime Death: 10 columns 4 rows
   mSlimeDeathAnim = {
       {DOWN, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}},
       {UP, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19}},
       {LEFT, {20, 21, 22, 23, 24, 25, 26, 27, 28, 29}},
       {RIGHT, {30, 31, 32, 33, 34, 35, 36, 37, 38, 39}}};

   // Create Player
   mGameState.player = new Entity({mOrigin.x + 500.0f, 400.0f}, {40.0f, 40.0f}, "assets/game/character_walk.png", ATLAS, {6, 4}, mWalkAnim, PLAYER);
   mGameState.player->setSpeed(150.0f);
   mGameState.player->setColliderDimensions({20.0f, 20.0f});
   mGameState.player->setHealth(5);
   mGameState.player->setAttackPower(1);

   // 6. Place Scenery (Trees)
   auto addTree = [&](Vector2 pos, Texture2D tex)
   {
      Entity *tree = new Entity(pos, {80.0f, 100.0f}, "", PROP);
      tree->setTexture(tex, 1, 1, {});
      tree->setColliderDimensions({30.0f, 55.0f});
      mGameState.scenery.push_back(tree);
   };

   // Top-left area
   addTree({100.0f, 100.0f}, mTexTree1);
   addTree({200.0f, 150.0f}, mTexTree2);
   addTree({350.0f, 80.0f}, mTexTree3);
   addTree({150.0f, 250.0f}, mTexTree4);
   addTree({450.0f, 200.0f}, mTexTree1);
   // Top-right area
   addTree({900.0f, 100.0f}, mTexTree2);
   addTree({1050.0f, 150.0f}, mTexTree3);
   addTree({800.0f, 250.0f}, mTexTree1);
   addTree({1100.0f, 300.0f}, mTexTree4);
   // Bottom-left area
   addTree({100.0f, 600.0f}, mTexTree3);
   addTree({250.0f, 650.0f}, mTexTree1);
   addTree({400.0f, 550.0f}, mTexTree2);
   addTree({150.0f, 750.0f}, mTexTree4);
   // Bottom-right area
   addTree({900.0f, 600.0f}, mTexTree1);
   addTree({1050.0f, 550.0f}, mTexTree4);
   addTree({850.0f, 700.0f}, mTexTree2);
   addTree({1100.0f, 750.0f}, mTexTree3);
   // Center area
   addTree({550.0f, 350.0f}, mTexTree4);
   addTree({650.0f, 450.0f}, mTexTree1);

   // ----- Create 6 Slimes -----
   Vector2 slimePositions[] = {
       {200.0f, 200.0f}, {800.0f, 200.0f}, {500.0f, 600.0f}, {1000.0f, 500.0f}, {300.0f, 500.0f}, {600.0f, 300.0f}};

   for (int i = 0; i < 6; i++)
   {
      Entity *slime = new Entity(slimePositions[i], {40.0f, 40.0f}, "assets/game/slime_walk.png", ATLAS, {8, 4}, mSlimeWalkAnim, ENEMY);
      slime->setAIType(SLIME);
      slime->setPatrolCenter(slimePositions[i]);
      slime->setColliderDimensions({25.0f, 25.0f});
      slime->setHealth(1);
      slime->setAttackPower(1);

      slime->setTexture(mTexSlimeWalk, 8, 4, mSlimeWalkAnim);
      mGameState.enemies.push_back(slime);
   }

   mGameState.camera.target = mGameState.player->getPosition();
   mGameState.camera.offset = mOrigin;
   mGameState.camera.zoom = 2.0f;
}

void LevelB::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   Entity *player = mGameState.player;
   if (player->getAIState() == DEAD)
      return;

   // Player dying state texture
   if (player->getAIState() == DYING)
   {
      player->setTexture(mTexPlayerDeath, 6, 4, mDeathAnim);
      player->update(deltaTime, player, mGameState.map, &mGameState.scenery);
      return;
   }

   // Player Logic
   if (IsKeyPressed(KEY_J) && player->getAIState() != ATTACKING)
   {
      player->setAIState(ATTACKING);
      player->setTexture(mTexPlayerAttack, 6, 4, mAttackAnim);
      player->setHasDealtDamage(false);
      PlaySound(mGameState.attackSound);
   }

   // Attack collision check
   if (player->getAIState() == ATTACKING)
   {
      int currentFrame = player->getCurrentFrameIndex();
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

   // Build obstacle list containing enemies and scenery
   std::vector<Entity *> playerObstacles = mGameState.scenery;
   for (Entity *enemy : mGameState.enemies)
   {
      if (enemy->getAIState() != DEAD && enemy->getAIState() != DYING)
         playerObstacles.push_back(enemy);
   }
   player->update(deltaTime, player, mGameState.map, &playerObstacles);

   // Switch to Level C
   float mapPixelHeight = LEVEL_HEIGHT_B * 50.0f;
   float mapTopEdge = mOrigin.y - mapPixelHeight / 2.0f;
   Vector2 pos = player->getPosition();
   Vector2 col = player->getColliderDimensions();
   bool inMiddleX = (pos.x > mOrigin.x - 100.0f) && (pos.x < mOrigin.x + 100.0f);
   if (inMiddleX && (pos.y - col.y / 2.0f <= mapTopEdge + 15.0f))
   {
      mGameState.nextSceneID = SCENE_LEVEL_C;
   }

   // Map Boundary
   float mapPixelWidth = LEVEL_WIDTH_B * 50.0f;
   if (pos.x - col.x / 2 < mOrigin.x - mapPixelWidth / 2)
      pos.x = mOrigin.x - mapPixelWidth / 2 + col.x / 2;
   if (pos.x + col.x / 2 > mOrigin.x + mapPixelWidth / 2)
      pos.x = mOrigin.x + mapPixelWidth / 2 - col.x / 2;
   if (pos.y - col.y / 2 < mOrigin.y - mapPixelHeight / 2)
      pos.y = mOrigin.y - mapPixelHeight / 2 + col.y / 2;
   if (pos.y + col.y / 2 > mOrigin.y + mapPixelHeight / 2)
      pos.y = mOrigin.y + mapPixelHeight / 2 - col.y / 2;
   player->setPosition(pos);

   for (Entity *prop : mGameState.scenery)
      prop->update(deltaTime, nullptr, mGameState.map, nullptr);

   // ------ Enemy Logic -------
   std::vector<Entity *> enemyObstacles = mGameState.scenery;
   enemyObstacles.push_back(player);
   for (Entity *otherEnemy : mGameState.enemies)
      if (otherEnemy->getAIState() != DEAD)
         enemyObstacles.push_back(otherEnemy);

   for (Entity *enemy : mGameState.enemies)
   {
      enemy->update(deltaTime, player, mGameState.map, &enemyObstacles);
      // Switch Slime texture based on state
      if (enemy->getAIState() == DYING)
         enemy->setTexture(mTexSlimeDeath, 10, 4, mSlimeDeathAnim); // 10 cols
      else if (enemy->getAIState() == ATTACKING)
         enemy->setTexture(mTexSlimeAttack, 10, 4, mSlimeAttackAnim); // 10 cols
      else
         enemy->setTexture(mTexSlimeWalk, 8, 4, mSlimeWalkAnim); // 8 cols

      // Enemy Attack Check
      if (enemy->getAIState() == ATTACKING)
      {
         if (enemy->getCurrentFrameIndex() == 4 && !enemy->hasDealtDamage())
         {
            Rectangle playerHurtBox = {
                player->getPosition().x - player->getColliderDimensions().x / 2.0f,
                player->getPosition().y - player->getColliderDimensions().y / 2.0f,
                player->getColliderDimensions().x, player->getColliderDimensions().y};

            if (CheckCollisionRecs(enemy->getAttackBox(), playerHurtBox))
            {
               if (player->getAIState() != DEAD && player->getAIState() != DYING)
               {
                  player->takeDamage(enemy->getAttackPower());
                  // Knockback logic
                  Vector2 knockback = Vector2Subtract(player->getPosition(), enemy->getPosition());
                  knockback = Vector2Normalize(knockback);
                  knockback = Vector2Scale(knockback, 20.0f);
                  player->setPosition(Vector2Add(player->getPosition(), knockback));
                  PlaySound(mGameState.hurtSound);
               }
            }
            enemy->setHasDealtDamage(true);
         }
      }
   }

   // Update Camera position
   float zoom = mGameState.camera.zoom;
   float screenWidth = (float)GetScreenWidth() / zoom;
   float screenHeight = (float)GetScreenHeight() / zoom;
   Vector2 cameraTarget = player->getPosition();
   float minTargetX = mOrigin.x - mapPixelWidth / 2 + screenWidth / 2;
   float maxTargetX = mOrigin.x + mapPixelWidth / 2 - screenWidth / 2;
   float minTargetY = mOrigin.y - mapPixelHeight / 2 + screenHeight / 2;
   float maxTargetY = mOrigin.y + mapPixelHeight / 2 - screenHeight / 2;
   cameraTarget.x = fmaxf(minTargetX, fminf(cameraTarget.x, maxTargetX));
   cameraTarget.y = fmaxf(minTargetY, fminf(cameraTarget.y, maxTargetY));
   mGameState.camera.target = cameraTarget;
}

void LevelB::render()
{
   ClearBackground(ColorFromHex("#405c36"));
   mGameState.map->render();
   // Put all entities into a list for sorted rendering
   std::vector<Entity *> allEntities;
   allEntities.push_back(mGameState.player);
   for (auto prop : mGameState.scenery)
      allEntities.push_back(prop);
   for (auto enemy : mGameState.enemies)
      allEntities.push_back(enemy);

   // Bubble sort by Y-axis (Achieve depth effect)
   for (size_t i = 0; i < allEntities.size(); i++)
   {
      for (size_t j = 0; j < allEntities.size() - 1; j++)
      {
         if (allEntities[j]->getPosition().y > allEntities[j + 1]->getPosition().y)
         {
            Entity *temp = allEntities[j];
            allEntities[j] = allEntities[j + 1];
            allEntities[j + 1] = temp;
         }
      }
   }
   for (Entity *e : allEntities)
      e->render();
}

void LevelB::shutdown()
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
   UnloadTexture(mTexSlimeWalk);
   UnloadTexture(mTexSlimeAttack);
   UnloadTexture(mTexSlimeDeath);
   UnloadTexture(mTexTree1);
   UnloadTexture(mTexTree2);
   UnloadTexture(mTexTree3);
   UnloadTexture(mTexTree4);
   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.attackSound);
   UnloadSound(mGameState.hurtSound);
}