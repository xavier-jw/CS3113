#include "LevelC.h"
#include "raymath.h"

// Map size: 14 columns x 12 rows
constexpr int LEVEL_WIDTH_C = 14;
constexpr int LEVEL_HEIGHT_C = 12;

constexpr unsigned int LEVEL_DATA_C[] = {
    2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
    2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
    2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
    2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
    2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
    2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
    2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
    2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
    2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
    2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
    2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
    2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};

void LevelC::initialise()
{
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
   // Load Map
   mGameState.map = new Map(LEVEL_WIDTH_C, LEVEL_HEIGHT_C, (unsigned int *)LEVEL_DATA_C,
                            "assets/game/tileset_2.png", 50.0f, 2, 1, mOrigin);

   mTexPlayerWalk = LoadTexture("assets/game/character_walk.png");
   mTexPlayerAttack = LoadTexture("assets/game/character_attack.png");
   mTexPlayerDeath = LoadTexture("assets/game/character_death.png");

   // Demon Textures
   mTexDemonWalk = LoadTexture("assets/game/demon_walk.png");
   mTexDemonAttack = LoadTexture("assets/game/demon_attack.png");
   mTexDemonDeath = LoadTexture("assets/game/demon_death.png");

   mTexTree1 = LoadTexture("assets/game/tree_1.png");
   mTexTree2 = LoadTexture("assets/game/tree_2.png");
   mTexTree3 = LoadTexture("assets/game/tree_3.png");
   mTexTree4 = LoadTexture("assets/game/tree_4.png");

   mGameState.bgm = LoadMusicStream("assets/audio/levelC_background.mp3");
   SetMusicVolume(mGameState.bgm, 0.5f);
   PlayMusicStream(mGameState.bgm);
   mGameState.attackSound = LoadSound("assets/audio/attack.wav");
   mGameState.hurtSound = LoadSound("assets/audio/hurt.wav");

   // Define Animations
   mWalkAnim = {{DOWN, {0, 1, 2, 3, 4, 5}}, {LEFT, {6, 7, 8, 9, 10, 11}}, {RIGHT, {12, 13, 14, 15, 16, 17}}, {UP, {18, 19, 20, 21, 22, 23}}};
   mAttackAnim = mWalkAnim;
   mDeathAnim = mWalkAnim;

   // Demon Walk: 6 columns 4 rows
   mDemonWalkAnim = {
       {DOWN, {0, 1, 2, 3, 4, 5}},
       {UP, {6, 7, 8, 9, 10, 11}},
       {LEFT, {12, 13, 14, 15, 16, 17}},
       {RIGHT, {18, 19, 20, 21, 22, 23}}};

   // Demon Attack: 10 columns 4 rows
   mDemonAttackAnim = {
       {DOWN, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}},
       {UP, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19}},
       {LEFT, {20, 21, 22, 23, 24, 25, 26, 27, 28, 29}},
       {RIGHT, {30, 31, 32, 33, 34, 35, 36, 37, 38, 39}}};

   // Demon Death: 13 columns 4 rows
   mDemonDeathAnim = {
       {DOWN, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}},
       {UP, {13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}},
       {LEFT, {26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38}},
       {RIGHT, {39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}}};

   // Create Player
   mGameState.player = new Entity({mOrigin.x, mOrigin.y + 200.0f}, {40.0f, 40.0f}, "assets/game/character_walk.png", ATLAS, {6, 4}, mWalkAnim, PLAYER);
   mGameState.player->setSpeed(150.0f);
   mGameState.player->setColliderDimensions({20.0f, 20.0f});
   mGameState.player->setHealth(5);
   mGameState.player->setAttackPower(1);

   // Place Scenery
   auto addTree = [&](Vector2 pos, Texture2D tex)
   {
      Entity *tree = new Entity(pos, {80.0f, 100.0f}, "", PROP);
      tree->setTexture(tex, 1, 1, {});
      tree->setColliderDimensions({30.0f, 55.0f});
      mGameState.scenery.push_back(tree);
   };

   // Left side trees
   addTree({mOrigin.x - 250.0f, mOrigin.y - 200.0f}, mTexTree1);
   addTree({mOrigin.x - 300.0f, mOrigin.y}, mTexTree2);
   addTree({mOrigin.x - 220.0f, mOrigin.y + 150.0f}, mTexTree3);
   addTree({mOrigin.x - 280.0f, mOrigin.y - 100.0f}, mTexTree4);
   // Right side trees
   addTree({mOrigin.x + 250.0f, mOrigin.y - 200.0f}, mTexTree2);
   addTree({mOrigin.x + 300.0f, mOrigin.y}, mTexTree3);
   addTree({mOrigin.x + 220.0f, mOrigin.y + 150.0f}, mTexTree1);
   addTree({mOrigin.x + 280.0f, mOrigin.y - 100.0f}, mTexTree4);

   // Create Boss (Demon)
   Vector2 bossPos = {mOrigin.x, mOrigin.y - 150.0f};
   Entity *demon = new Entity(bossPos, {80.0f, 80.0f}, "assets/game/demon_walk.png", ATLAS, {6, 4}, mDemonWalkAnim, ENEMY);

   demon->setAIType(DEMON);
   demon->setColliderDimensions({40.0f, 40.0f});
   // Boss Stats: 6 Health, 2 Attack Power
   demon->setHealth(6);
   demon->setAttackPower(2);

   demon->setTexture(mTexDemonWalk, 6, 4, mDemonWalkAnim);

   mGameState.enemies.push_back(demon);

   mGameState.camera.target = mGameState.player->getPosition();
   mGameState.camera.offset = mOrigin;
   mGameState.camera.zoom = 2.0f;
}

void LevelC::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   Entity *player = mGameState.player;
   if (player->getAIState() == DEAD)
      return;

   // Player death logic
   if (player->getAIState() == DYING)
   {
      player->setTexture(mTexPlayerDeath, 6, 4, mDeathAnim);
      player->update(deltaTime, player, mGameState.map, &mGameState.scenery);
      return;
   }

   // Player Attack Logic
   if (IsKeyPressed(KEY_J) && player->getAIState() != ATTACKING)
   {
      player->setAIState(ATTACKING);
      player->setTexture(mTexPlayerAttack, 6, 4, mAttackAnim);
      player->setHasDealtDamage(false);
      PlaySound(mGameState.attackSound);
   }

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
                  // Boss is heavy, knockback distance is smaller (10.0f)
                  knockback = Vector2Scale(knockback, 10.0f);
                  enemy->setPosition(Vector2Add(enemy->getPosition(), knockback));
               }
            }
         }
         player->setHasDealtDamage(true);
      }
   }

   if (player->getAIState() == IDLE)
      player->setTexture(mTexPlayerWalk, 6, 4, mWalkAnim);

   // Build obstacle list including Boss
   std::vector<Entity *> playerObstacles = mGameState.scenery;
   for (Entity *enemy : mGameState.enemies)
   {
      if (enemy->getAIState() != DEAD && enemy->getAIState() != DYING)
         playerObstacles.push_back(enemy);
   }
   player->update(deltaTime, player, mGameState.map, &playerObstacles);

   float mapPixelWidth = LEVEL_WIDTH_C * 50.0f;
   float mapPixelHeight = LEVEL_HEIGHT_C * 50.0f;
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

   mGameState.camera.target = player->getPosition();

   for (Entity *prop : mGameState.scenery)
      prop->update(deltaTime, nullptr, mGameState.map, nullptr);

   std::vector<Entity *> enemyObstacles = mGameState.scenery;
   enemyObstacles.push_back(player);

   // Boss Logic Update
   for (Entity *enemy : mGameState.enemies)
   {
      enemy->update(deltaTime, player, mGameState.map, &enemyObstacles);
      // Switch Boss textures
      if (enemy->getAIState() == DYING)
         enemy->setTexture(mTexDemonDeath, 13, 4, mDemonDeathAnim);
      else if (enemy->getAIState() == ATTACKING)
         enemy->setTexture(mTexDemonAttack, 10, 4, mDemonAttackAnim);
      else
         enemy->setTexture(mTexDemonWalk, 6, 4, mDemonWalkAnim);

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
                  Vector2 knockback = Vector2Subtract(player->getPosition(), enemy->getPosition());
                  knockback = Vector2Normalize(knockback);
                  knockback = Vector2Scale(knockback, 15.0f);
                  player->setPosition(Vector2Add(player->getPosition(), knockback));
                  PlaySound(mGameState.hurtSound);
               }
            }
            enemy->setHasDealtDamage(true);
         }
      }
   }

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

   // Victory Condition
   bool allEnemiesDead = true;
   for (Entity *enemy : mGameState.enemies)
   {
      if (enemy->getAIState() != DEAD)
      {
         allEnemiesDead = false;
         break;
      }
   }

   if (allEnemiesDead)
   {
      mGameState.nextSceneID = SCENE_WIN;
   }
}

void LevelC::render()
{
   ClearBackground(ColorFromHex("#405c36"));
   mGameState.map->render();
   std::vector<Entity *> allEntities;
   allEntities.push_back(mGameState.player);
   for (auto prop : mGameState.scenery)
      allEntities.push_back(prop);
   for (auto enemy : mGameState.enemies)
      allEntities.push_back(enemy);

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

void LevelC::shutdown()
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
   UnloadTexture(mTexDemonWalk);
   UnloadTexture(mTexDemonAttack);
   UnloadTexture(mTexDemonDeath);
   UnloadTexture(mTexTree1);
   UnloadTexture(mTexTree2);
   UnloadTexture(mTexTree3);
   UnloadTexture(mTexTree4);
   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.attackSound);
   UnloadSound(mGameState.hurtSound);
}