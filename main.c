#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#define PLAYER_MAX_LIVES 3
#define HERO_HEIGHT      50
#define HERO_WIDTH       100
#define ENEMY_HEIGHT     32
#define ENEMY_WIDTH      32
#define ENEMIES_PER_LINE 10
#define ENEMY_SHOOT_TIME 60

// User structure
typedef struct Hero {
    Rectangle rec;
    Vector2 speed;
    int score;
    int lives;
} Hero;

// Shoot (used by hero, invader and enemy ship)
typedef struct Shoot {
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
} Shoot;

typedef struct Defense {
    Rectangle structure[6];
    bool active[6];
} Defense;

// Normal enemy
typedef struct Invader {
    Texture2D sprite;
    Rectangle bounds;
    Vector2 speed;
    bool active;
    Color color;
    int value;
} Invader;

// Special enemy (mothersip)
typedef struct EnemyShip {
    Texture2D sprite;
    Rectangle bounds;
    Vector2 speed;
    bool active;
    int value;
    bool isEnemyShipDestroyed;
} EnemyShip;

// Option of the menu
typedef struct MenuOption {
    char *text;
    Vector2 pos;
} MenuOption;

// Arrays and pointers (also arrays lol) for the game
MenuOption options[3];
Invader *octopus;
Invader *crabFirstLine;
Invader *crabSecondLine;
Invader *squidFirstLine;
Invader *squidSecondLine;
Defense *defenses;
Hero *hero;
Shoot *heroShoot;
Shoot *invaderShoot;
Shoot *mothsershipShoot;
EnemyShip *mothership;

// Window dimensions
const int screenWidth = 1200;
const int screenHeight = 975;

void InitGame();
void ExecuteGameLoop();
void UpdateGame();
void RenderSpaceInvaders();
void MovePlayer(bool positive);
void HeroShoot();
void CheckEnemyCollision();
void MoveEnemies();
void EnemyShoot();
void CheckDefenseCollision();
void CheckHeroCollision();
void RenderDefenses();
void RenderMenu();
void SaveGame();
void LoadGame();
void FreeGameMemory();
void MothershipLogic();
void ResumeGame();

// Flags for the game
bool moveEnemiesToLeft = true;
bool pause = false;
bool isUserDeath = false; // <= This one could be inside of Hero structure
bool menu = false;
bool moveMothershipToLeft = true;
bool allEnemiesAreDeath;

// Textures for the game (enemy images)
Texture2D octopusTexture;
Texture2D crabTexture;
Texture2D squidTexture;
Texture2D EnemyShipTexture;

int add = 0;
int frame = 0;
int frameForEnemyShip = 0;
int enemyShipTime = 0;
int currentLevel = 0;

int main() {
    // Create the window
    InitWindow(screenWidth, screenHeight, "Space Invaders");

    // Load the textures (images) for the enemies
    octopusTexture = LoadTexture("resources/Octopus.png");
    crabTexture = LoadTexture("resources/Crab.png");
    squidTexture = LoadTexture("resources/Squid.png");
    EnemyShipTexture = LoadTexture("resources/EnemyShip.png");
    InitGame(true);

    // Determine random time for the enemy ship to appear
    enemyShipTime = rand() % (70-61) + 60;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        // Set background color
        ClearBackground(BLACK);

        // The real "main function" for the game logic
        ExecuteGameLoop();

        EndDrawing();
    }

    // Free the memory
    FreeGameMemory();

    // End the game
    CloseWindow();

    return 0;
}

void InitGame(bool firstGame)
{
    // If first game false then we are going to init the next level
    if (!firstGame) currentLevel++;
    else currentLevel = 0; // if first game is true the current level is 0 (level 1)

    if (firstGame)
    {
        hero = malloc(sizeof(struct Hero));
        // Init hero
        hero->speed.x = -2.f;
        hero->rec.height = HERO_HEIGHT;
        hero->rec.width = HERO_WIDTH;
        hero->rec.x = (float)screenWidth / 2;
        hero->rec.y = (float)screenHeight - HERO_HEIGHT * 2 - 50;
        hero->score = 0;
        hero->lives = PLAYER_MAX_LIVES;

        heroShoot = malloc(sizeof(struct Shoot));
        // Init hero shoot
        heroShoot->color = RED;
        heroShoot->rec.height = 20.f;
        heroShoot->rec.width = 5.f;
        heroShoot->speed.y = 7.f;
        heroShoot->active = false;
    }

    invaderShoot = malloc(sizeof(struct Shoot));
    // Init enemy shoot
    invaderShoot->rec.height = 20.f;
    invaderShoot->rec.width = 5.f;
    invaderShoot->color = WHITE;
    invaderShoot->speed.y = 7.f;
    invaderShoot->active = false;

    // Variables used to render the grid of enemies, offset to the top and sides margins
    int screenOffset = 75;
    int lineNumber = 1;

    octopus =  malloc(sizeof(Invader) * ENEMIES_PER_LINE);
    // Init Octopus array
    Invader defaultOctopus = {
        .active = true,
        .color = PINK,
        .speed = { .x = 1.5f + currentLevel, .y = (float) ENEMY_HEIGHT / 2 },
        .sprite = octopusTexture,
        .bounds = { .height = 32, .width = 48 },
        .value = 30
    };

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        octopus[i] = defaultOctopus;
        octopus[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        octopus[i].bounds.y = ENEMY_HEIGHT * lineNumber * 1.25;
    }
    lineNumber++;

    crabFirstLine =  malloc(sizeof(Invader) * ENEMIES_PER_LINE);
    // Init Crab array
    Invader defaultCrab = {
        .active = true,
        .color = SKYBLUE,
        .speed = { .x = 1.5f + currentLevel, .y = (float) ENEMY_HEIGHT / 2 },
        .sprite = crabTexture,
        .bounds = { .height = 32, .width = 48 },
        .value = 20
    };

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        crabFirstLine[i] = defaultCrab;
        crabFirstLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        crabFirstLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 1.4;
    }
    lineNumber++;

    crabSecondLine =  malloc(sizeof(Invader) * ENEMIES_PER_LINE);
    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        crabSecondLine[i] = defaultCrab;
        crabSecondLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        crabSecondLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 1.4;
    }
    lineNumber++;

    // Init Squid array
    Invader defaultSquid = {
        .active = true,
        .color = GREEN,
        .speed = { .x = 1.5f + currentLevel, .y = (float) ENEMY_HEIGHT / 2 },
        .sprite = squidTexture,
        .bounds = { .height = 32, .width = 48 },
        .value = 10
    };

    squidFirstLine =  malloc(sizeof(Invader) * ENEMIES_PER_LINE);
    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        squidFirstLine[i] = defaultSquid;
        squidFirstLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidFirstLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 1.4;
    }
    lineNumber++;

    squidSecondLine =  malloc(sizeof(Invader) * ENEMIES_PER_LINE);
    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        squidSecondLine[i] = defaultSquid;
        squidSecondLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidSecondLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 1.5;
    }

    allEnemiesAreDeath = false;

    defenses = malloc(sizeof(Defense) * 3);
    // Init Defenses (with an horrible and ultra explicit in line for)
    for (int i = 0; i < 3; i++) {
        defenses[i].active[0] = true;
        defenses[i].active[1] = true;
        defenses[i].active[2] = true;
        defenses[i].active[3] = true;
        defenses[i].active[4] = true;
        defenses[i].active[5] = true;
        defenses[i].structure[0].width = 35;
        defenses[i].structure[0].height = 35;
        defenses[i].structure[1].width = 35;
        defenses[i].structure[1].height = 35;
        defenses[i].structure[2].width = 35;
        defenses[i].structure[2].height = 35;
        defenses[i].structure[3].width = 35;
        defenses[i].structure[3].height = 35;
        defenses[i].structure[4].width = 35;
        defenses[i].structure[4].height = 35;
        defenses[i].structure[5].width = 35;
        defenses[i].structure[5].height = 35;
        defenses[i].structure[0].x = 100.f + 430.f * (float)i;
        defenses[i].structure[0].y = (float)screenHeight - 250.f;
        defenses[i].structure[1].x = 100.f + 430.f * (float)i;
        defenses[i].structure[1].y = (float)screenHeight - 285.f;
        defenses[i].structure[2].x = 135.f + 430.f * (float)i;
        defenses[i].structure[2].y = (float)screenHeight - 285.f;
        defenses[i].structure[3].x = 170.f + 430.f * (float)i;
        defenses[i].structure[3].y = (float)screenHeight - 285.f;
        defenses[i].structure[4].x = 205.f + 430.f * (float)i;
        defenses[i].structure[4].y = (float)screenHeight - 285.f;
        defenses[i].structure[5].x = 205.f + 430.f * (float)i;
        defenses[i].structure[5].y = (float)screenHeight - 250.f;
    }

    // Init mothership
    mothership = malloc(sizeof(EnemyShip));
    mothership->sprite = EnemyShipTexture;
    mothership->active = false;
    mothership->bounds.y = 10;
    mothership->bounds.height = 32;
    mothership->bounds.width = 48;
    mothership->speed.x = 4.f;
    mothership->value = 0;
    mothership->isEnemyShipDestroyed = false;

    // Init mothership shoot
    mothsershipShoot = malloc(sizeof(Shoot));
    mothsershipShoot->active = false;
    mothsershipShoot->speed.y = 10.f;
    mothsershipShoot->rec.width = 8.f;
    mothsershipShoot->rec.height = 20.f;
    mothsershipShoot->color = GREEN;

    SetTargetFPS(60);
}

void ExecuteGameLoop()
{
    UpdateGame();
    RenderSpaceInvaders();
}

void UpdateGame()
{
    // Check if it the pause or menu key is pressed
    if (IsKeyPressed(KEY_P)) pause = !pause;
    if (IsKeyPressed(KEY_M)) menu = !menu;

    if (!pause && !menu && !isUserDeath) {
        // Hero movement keys
        if (IsKeyDown(KEY_RIGHT)) MovePlayer(false);
        if (IsKeyDown(KEY_LEFT)) MovePlayer(true);

        // Hero shoot trigger
        if (IsKeyDown(KEY_SPACE) && !heroShoot->active) HeroShoot();

        // Shoot displacement and collision
        if (heroShoot->active)
        {
            heroShoot->rec.y = heroShoot->rec.y - heroShoot->speed.y;

            // The user is able to shoot only when the previous shoot
            // collides with an enemy or "collides" with the end of the screen
            if (heroShoot->rec.y < 0) {
                heroShoot->active = false;
            } else {
                CheckEnemyCollision();
            }
        }

        // Shoot displacement and collision
        if (invaderShoot->active)
        {
            invaderShoot->rec.y += invaderShoot->speed.y;

            if (invaderShoot->rec.y >= (double)screenHeight) {
                invaderShoot->active = false;
                invaderShoot->rec.y = 0;
            } else {
                CheckHeroCollision();
            }
        }

        // Shoot displacement and collision
        if (mothsershipShoot->active)
        {
            mothsershipShoot->rec.y += mothsershipShoot->speed.y;
            if (mothsershipShoot->rec.y >= (double)screenHeight) {
                mothsershipShoot->active = false;
                mothsershipShoot->rec.y = 35;
            }
        }

        CheckDefenseCollision();
        EnemyShoot();
        MothershipLogic();
        MoveEnemies();

        // Go to next level when all the enemies (of the grid) are death
        if (allEnemiesAreDeath) InitGame(false);
    }

    // When the user get a shot
    if (isUserDeath)
    {
        // Check if the user have lives
        if (hero->lives > 0)
        {
            // Show continue instructions
            DrawText("Presiona \nbarra espaciadora \npara continuar", screenHeight / 4, screenWidth / 4, 100, GREEN);
            if (IsKeyDown(KEY_SPACE)) isUserDeath = false;
        }
        else {
            // Show game over feedback and instructions to reset
            DrawText("Game over", screenHeight / 4, screenWidth / 4, 100, RED);
            DrawText("Insert coin to continue\n (press R)", screenHeight / 5, screenWidth / 4 + 150, 80, GREEN);
            if (IsKeyDown(KEY_R))
            {
                // Reset the game
                InitGame(true);
                isUserDeath = false;
            }
        }
    }
}

void RenderSpaceInvaders()
{
    if (pause) DrawText(TextFormat("GAME PAUSED"), screenWidth / 2 - 150, screenHeight - 50, 40, DARKBLUE);

    DrawText(TextFormat("Score: %d", hero->score), 10, 5, 24, GREEN);
    DrawText(TextFormat("Level: %d", currentLevel + 1), screenWidth - 100, 5, 24, GREEN);
    DrawText(TextFormat("PRESS M TO GO TO MENU"), screenWidth-300, screenHeight-30, 20, GRAY);

    if (mothership->active) DrawTexture(mothership->sprite, (int)mothership->bounds.x, (int)mothership->bounds.y, WHITE);

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (octopus[i].active) DrawTexture(octopus[i].sprite, (int)octopus[i].bounds.x, (int)octopus[i].bounds.y , octopus[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (crabFirstLine[i].active) DrawTexture(crabFirstLine[i].sprite, (int)crabFirstLine[i].bounds.x, (int)crabFirstLine[i].bounds.y, crabFirstLine[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (crabSecondLine[i].active) DrawTexture(crabSecondLine[i].sprite, (int)crabSecondLine[i].bounds.x, (int)crabSecondLine[i].bounds.y, crabSecondLine[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (squidFirstLine[i].active) DrawTexture(squidFirstLine[i].sprite, (int)squidFirstLine[i].bounds.x, (int)squidFirstLine[i].bounds.y, squidFirstLine[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (squidSecondLine[i].active) DrawTexture(squidSecondLine[i].sprite, (int)squidSecondLine[i].bounds.x, (int)squidSecondLine[i].bounds.y, squidSecondLine[i].color);
    }

    if (heroShoot->active)
    {
        DrawRectangleRec(heroShoot->rec, heroShoot->color);
    }

    if (invaderShoot->active)
    {
        DrawRectangleRec(invaderShoot->rec, invaderShoot->color);
    }

    if (mothsershipShoot->active)
    {
        DrawRectangleRec(mothsershipShoot->rec, mothsershipShoot->color);
    }

    RenderDefenses();

    DrawRectangleRec(hero->rec, GREEN);
    for(int i = 0; i < hero->lives - 1; i++)
    {
        DrawRectangle(i * 100 + (i * 10) + 10, GetScreenHeight() - HERO_HEIGHT - 10, HERO_WIDTH, HERO_HEIGHT, GREEN);
    }

    RenderMenu();
}

// Function to move the player
void MovePlayer(bool positive)
{
    hero->rec.x += positive ? hero->speed.x : -hero->speed.x;
}

// Active hero shoot
void HeroShoot()
{
    if (!heroShoot->active) // <= Double validation?
    {
        heroShoot->active = true;
        heroShoot->rec.x = hero->rec.x + HERO_WIDTH / 2.f;
        heroShoot->rec.y = hero->rec.y - heroShoot->rec.height + 5;
    }
}

// Check if the heroShoot collides with an enemy (squid, crab, octopus or mothership)
void CheckEnemyCollision()
{
    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (octopus[i].active)
        {
            if (CheckCollisionRecs(heroShoot->rec, octopus[i].bounds))
            {
                heroShoot->active = false;
                octopus[i].active = false;
                hero->score += octopus[i].value;
            }
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (crabFirstLine[i].active)
        {
            if (CheckCollisionRecs(heroShoot->rec, crabFirstLine[i].bounds))
            {
                heroShoot->active = false;
                crabFirstLine[i].active = false;
                hero->score += crabFirstLine[i].value;
            }
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (crabSecondLine[i].active)
        {
            if (CheckCollisionRecs(heroShoot->rec, crabSecondLine[i].bounds))
            {
                heroShoot->active = false;
                crabSecondLine[i].active = false;
                hero->score += crabSecondLine[i].value;
            }
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (squidFirstLine[i].active)
        {
            if (CheckCollisionRecs(heroShoot->rec, squidFirstLine[i].bounds))
            {
                heroShoot->active = false;
                squidFirstLine[i].active = false;
                hero->score += squidFirstLine[i].value;
            }
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (squidSecondLine[i].active)
        {
            if (CheckCollisionRecs(heroShoot->rec, squidSecondLine[i].bounds))
            {
                heroShoot->active = false;
                squidSecondLine[i].active = false;
                hero->score += squidSecondLine[i].value;
            }
        }
    }
    if (mothership->active)
    {
        if (CheckCollisionRecs(heroShoot->rec, mothership->bounds))
        {
            heroShoot->active = false;
            mothership->active = false;
            mothership->isEnemyShipDestroyed = true;
            hero->score += rand() % (100 - 61) + 60;
        }
    }
}

// Move the enemies in x and y axis
void MoveEnemies()
{
    bool moveEnemiesInYAxis = false;
    allEnemiesAreDeath = true;

    // Check if we are in the border of the screen (right border)
    for(int i = 9; i >= 0; i--)
    {
        if (octopus[i].active)
        {
            allEnemiesAreDeath = false;
            if (octopus[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (crabFirstLine[i].active)
        {
            allEnemiesAreDeath = false;
            if (crabFirstLine[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (crabSecondLine[i].active)
        {
            allEnemiesAreDeath = false;
            if (crabSecondLine[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (squidFirstLine[i].active)
        {
            allEnemiesAreDeath = false;
            if (squidFirstLine[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (squidSecondLine[i].active)
        {
            allEnemiesAreDeath = false;
            if (squidSecondLine[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
    }

    // Check if we are in the border of the screen (left border)
    for(int i = 0; i < 10; i++)
    {
        if (octopus[i].active)
        {
            if (octopus[i].bounds.x <= 0)
            {
                moveEnemiesToLeft = true;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (crabFirstLine[i].active)
        {
            if (crabFirstLine[i].bounds.x <= 0)
            {
                moveEnemiesToLeft = true;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (crabSecondLine[i].active)
        {
            if (crabSecondLine[i].bounds.x <= 0)
            {
                moveEnemiesToLeft = true;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (squidFirstLine[i].active)
        {
            if (squidFirstLine[i].bounds.x <= 0)
            {
                moveEnemiesToLeft = true;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (squidSecondLine[i].active)
        {
            if (squidSecondLine[i].bounds.x <= 0)
            {
                moveEnemiesToLeft = true;
                moveEnemiesInYAxis = true;
            }
            break;
        }
    }

    // Move the enemies
    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        // Move enemies in X axis
        if (!moveEnemiesInYAxis) octopus[i].bounds.x += moveEnemiesToLeft ? octopus[i].speed.x : -octopus[i].speed.x;
        // Move enemies in Y axis
        else
            {
            octopus[i].bounds.y += octopus[i].speed.y;
            octopus[i].bounds.x += moveEnemiesToLeft ? 1 : -1;

            // If we hit this arbitrary defense (all are at the same height so we can take any rectangle of any defense structure) user is death
            if (octopus[i].active && octopus[i].bounds.y >= defenses[2].structure[3].y - ENEMY_HEIGHT - 35)
            {
                hero->lives = 0;
                isUserDeath = true;
            }
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {

        if (!moveEnemiesInYAxis) crabFirstLine[i].bounds.x += moveEnemiesToLeft ? crabFirstLine[i].speed.x : -crabFirstLine[i].speed.x;
        else
        {
            crabFirstLine[i].bounds.y += crabFirstLine[i].speed.y;
            crabFirstLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;

            // If we hit this arbitrary defense (all are at the same height so we can take any rectangle of any defense structure) user is death
            if (crabFirstLine[i].active && crabFirstLine[i].bounds.y >= defenses[2].structure[2].y - ENEMY_HEIGHT - 35)
            {
                hero->lives = 0;
                isUserDeath = true;
            }
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (!moveEnemiesInYAxis) crabSecondLine[i].bounds.x += moveEnemiesToLeft ? crabSecondLine[i].speed.x : -crabSecondLine[i].speed.x;
        else
        {
            crabSecondLine[i].bounds.y += crabSecondLine[i].speed.y;
            crabSecondLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;

            // If we hit this arbitrary defense (all are at the same height so we can take any rectangle of any defense structure) user is death
            if (crabSecondLine[i].active && crabSecondLine[i].bounds.y >= defenses[2].structure[2].y - ENEMY_HEIGHT - 20)
            {
                hero->lives = 0;
                isUserDeath = true;
            }
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (!moveEnemiesInYAxis) squidFirstLine[i].bounds.x += moveEnemiesToLeft ? squidFirstLine[i].speed.x : -squidFirstLine[i].speed.x;
        else
        {
            squidFirstLine[i].bounds.y += squidFirstLine[i].speed.y;
            squidFirstLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;

            // If we hit this arbitrary defense (all are at the same height so we can take any rectangle of any defense structure) user is death
            if (squidFirstLine[i].active && squidFirstLine[i].bounds.y >= defenses[2].structure[2].y - ENEMY_HEIGHT - 20)
            {
                hero->lives = 0;
                isUserDeath = true;
            }
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (!moveEnemiesInYAxis) squidSecondLine[i].bounds.x += moveEnemiesToLeft ? squidSecondLine[i].speed.x : -squidSecondLine[i].speed.x;
        else
        {
            squidSecondLine[i].bounds.y += squidSecondLine[i].speed.y;
            squidSecondLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;

            // If we hit this arbitrary defense (all are at the same height so we can take any rectangle of any defense structure) user is death
            if (squidSecondLine[i].active && squidSecondLine[i].bounds.y >= defenses[2].structure[2].y - ENEMY_HEIGHT - 20)
            {
                hero->lives = 0;
                isUserDeath = true;
            }
        }
    }
}

void EnemyShoot()
{
    // If the shoot is not active
    if (!invaderShoot->active)
    {
        // Update the frame variable
        frame++;

        // If the frame is greater or equal than the time we want to wait
        if (frame >= ENEMY_SHOOT_TIME)
        {

            // We use this for to determine which enemy should shoot
            // Checking every active enemy and their position
            for (int i = 0; i < ENEMIES_PER_LINE; i++) {
                Invader enemy;
                enemy.active = false;
                if (squidSecondLine[i].active)
                {
                    enemy = squidSecondLine[i];
                } else if (squidFirstLine[i].active)
                {
                    enemy = squidFirstLine[i];
                } else if (crabSecondLine[i].active)
                {
                    enemy = crabSecondLine[i];
                } else if (crabFirstLine[i].active)
                {
                    enemy = crabFirstLine[i];
                } else if (octopus[i].active)
                {
                    enemy = octopus[i];
                }

                if (enemy.active && enemy.bounds.x >= hero->rec.x - (HERO_WIDTH / 2.f) && enemy.bounds.x <= hero->rec.x + (HERO_WIDTH / 2.f))
                {
                    invaderShoot->rec.y = enemy.bounds.y + enemy.bounds.height;
                    invaderShoot->rec.x = enemy.bounds.x + (enemy.bounds.width / 2.f);
                    invaderShoot->active = true;
                }
            }
        }
    } else {
        // Set the frame to 0, so when we deactivate the shoot the frame is in 0
        frame = 0;
    }
}

// Check if an enemy shoot hit the hero
void CheckHeroCollision()
{
    if (invaderShoot->active)
    {
        if (CheckCollisionRecs(invaderShoot->rec, hero->rec))
        {
            invaderShoot->active = false;
            hero->lives--;
            isUserDeath = true;
        }
    }
    if (mothsershipShoot->active)
    {
        if (CheckCollisionRecs(mothsershipShoot->rec, hero->rec))
        {
            mothsershipShoot->active = false;
            hero->lives--;
            isUserDeath = true;
        }
    }
}

// Check if any shoot (from the enemy or hero) hit a defense rectangle
void CheckDefenseCollision()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (defenses[i].active[j])
            {
                if (heroShoot->active)
                {
                    if (CheckCollisionRecs(heroShoot->rec, defenses[i].structure[j]))
                    {
                        heroShoot->active = false;
                        defenses[i].active[j] = false;
                    }
                }
                if (invaderShoot->active)
                {
                    if (CheckCollisionRecs(invaderShoot->rec, defenses[i].structure[j]))
                    {
                        invaderShoot->active = false;
                        defenses[i].active[j] = false;
                    }
                }
                if (mothsershipShoot->active)
                {
                    if (CheckCollisionRecs(mothsershipShoot->rec, defenses[i].structure[j]))
                    {
                        mothsershipShoot->active = false;
                        defenses[i].active[j] = false;
                    }
                }
            }
        }
    }
}

void RenderDefenses()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 6; j++) {
            if (defenses[i].active[j])
            {
                DrawRectangleRec(defenses[i].structure[j], GREEN);
            }
        }
    }
}

void RenderMenu()
{
    if (menu) {
        DrawRectangleRec(hero->rec, GREEN);
        char *menuLetters[] = {"-> SAVE GAME", "-> LOAD GAME", "-> RESUME"};
        MenuOption menuOp[3];
        int pos = (screenHeight/2)-125+(add*40);

        DrawRectangle((screenWidth/2)-200, (screenHeight/2)-135, 400, 270, BLACK);
        DrawRectangle((screenWidth/2)-180, (screenHeight/2)-125+(add*40), 350, 40, GRAY);

        if(IsKeyReleased((KEY_UP)) && add > 0) add--;
        if(IsKeyReleased((KEY_DOWN)) && add < 3) add++;

        for(int i = 0; i < 3; i++) {
            menuOp[i].text = menuLetters[i];
            menuOp[i].pos.x = ((float)screenWidth/2)-180;
            menuOp[i].pos.y = ((float)screenHeight/2)-125+((float)i*40);
            DrawText(menuOp[i].text, (int)menuOp[i].pos.x, (int)menuOp[i].pos.y, 40, WHITE);
        }

        if((pos == ((screenHeight/2)-125+(0*40))) && (IsKeyDown(KEY_ENTER))) SaveGame();
        if((pos == ((screenHeight/2)-125+(1*40))) && (IsKeyDown(KEY_ENTER))) LoadGame();
        if((pos == ((screenHeight/2)-125+(2*40))) && (IsKeyDown(KEY_ENTER))) ResumeGame();
    }
}

void SaveGame()
{
    // Array to save all the enemies of the game
    Invader gameToSave[50];
    int j = 0;

    // Save an array of enemies
    for (int i = 0; i < 5; i++)
    {
        Invader *enemy;
        // Set enemy
        switch (i) {
            case 0:
                enemy = octopus;
                break;
            case 1:
                enemy = crabFirstLine;
                break;
            case 2:
                enemy = crabSecondLine;
                break;
            case 3:
                enemy = squidFirstLine;
                break;
            case 4:
                enemy = squidSecondLine;
                break;
            default:
                break;
        }
        int ref = 0;
        for (; j < ENEMIES_PER_LINE * (i+1); j++)
        {
            // Save enemy on the array
            gameToSave[j] = enemy[ref];
            ref++;
        }
    }

    // Save all the data on Enemies.bin (Ignore the name, it will work)
    FILE *fileW = fopen("Enemies.bin", "wb");
    fwrite(gameToSave, sizeof(struct Invader), 50, fileW);
    fwrite(invaderShoot, sizeof(struct Shoot), 1, fileW);
    fwrite(hero, sizeof(struct Hero), 1, fileW);
    fwrite(heroShoot, sizeof(struct Shoot), 1, fileW);
    fwrite(defenses, sizeof(struct Defense), 3, fileW);
    fwrite(&currentLevel, sizeof(int), 3, fileW);
    fclose(fileW);
}

void LoadGame()
{
    // Array to save all the enemies
    Invader enemies[50];

    // Open Enemies.bin (Ignore the name, it will work)
    FILE *file = fopen("Enemies.bin", "rb");
    if (file != NULL)
    {
        // Load all the enemies
        for (int i = 0; i < 50; i++)
        {
            fread(&enemies[i], sizeof(struct Invader), 1, file);
        }

        int j = 0;
        // Save enemies on their correspondent arrays
        for (int i = 0; i < 50; i++) {
            if (j == 10) j = 0;
            if (i < 10) octopus[j] = enemies[i];
            else if (i < 20) squidFirstLine[j] = enemies[i];
            else if (i < 30) squidSecondLine[j] = enemies[i];
            else if (i < 40) crabFirstLine[j] = enemies[i];
            else crabSecondLine[j] = enemies[i];
            j++;
        }

        // Load the rest of the game
        fread(invaderShoot, sizeof(struct Shoot), 1, file);

        fread(hero, sizeof(struct Hero), 1, file);

        fread(heroShoot, sizeof(struct Shoot), 1, file);

        for (int i = 0; i < 3; i++) {
            fread(&defenses[i], sizeof(struct Defense), 1, file);
        }

        fread(&currentLevel, sizeof(int), 1, file);

        fclose(file);
    }
}

// Free all the memory used
void FreeGameMemory()
{
    free(octopus);
    free(crabFirstLine);
    free(crabSecondLine);
    free(squidFirstLine);
    free(squidSecondLine);
    free(defenses);
}

void MothershipLogic()
{
    // If the mothership was not destroyed yet in the current level
    if (!mothership->isEnemyShipDestroyed)
    {
        // If the mothership is not active
        if (!mothership->active)
        {
            // Update the frame
            frameForEnemyShip++;
            if (frame >= enemyShipTime)
            {
                // Active the mothership
                mothership->active = true;
            }

            // Set initial position
            mothership->bounds.x = moveMothershipToLeft ? 0 : (float)screenWidth - 50;
        } else
        {
            // Set to 0
            frameForEnemyShip = 0;
            // Move the ship
            mothership->bounds.x += moveMothershipToLeft ? mothership->speed.x : -mothership->speed.x;
            // Determine the right moment to shoot at the user position
            if (mothership->bounds.x >= hero->rec.x - (HERO_WIDTH / 2.f) && mothership->bounds.x <= hero->rec.x + (HERO_WIDTH / 2.f))
            {
                mothsershipShoot->rec.x = mothership->bounds.x + 20;
                mothsershipShoot->rec.y = 35 + mothership->bounds.y;
                mothsershipShoot->active = true;
            }
            // Detect border collision
            else if (mothership->bounds.x >= (float) screenWidth - 50)
            {
                mothership->active = false;
                moveMothershipToLeft = false;
            } else if (mothership->bounds.x <= 0)
            {
                mothership->active = false;
                moveMothershipToLeft = true;
            }
        }
    }
}

void ResumeGame() {
    menu = !menu;
}