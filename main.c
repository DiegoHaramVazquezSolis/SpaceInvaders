#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "raylib.h"

#define PLAYER_MAX_LIFE  3
#define HERO_HEIGHT      50
#define HERO_WIDTH       100
#define ENEMY_HEIGHT     40
#define ENEMY_WIDTH      40
#define ENEMIES_PER_LINE 10

typedef struct Hero {
    Rectangle rec;
    Vector2 speed;
} Hero;

typedef struct Shoot {
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
} Shoot;

typedef struct Octopus {
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
    int value;
    int index;
} Octopus;

typedef struct Crab {
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
    int value;
    int index;
} Crab;

typedef struct Squid {
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
    int value;
    int index;
} Squid;

typedef struct EnemyShip {
    Rectangle rec;
    Vector2 speed;
    bool active;
    int value;
} EnemyShip;

Octopus octopus[ENEMIES_PER_LINE] = { 0 };
Crab crabFirstLine[ENEMIES_PER_LINE] = { 0 };
Crab crabSecondLine[ENEMIES_PER_LINE] = { 0 };
Squid squidFirstLine[ENEMIES_PER_LINE] = { 0 };
Squid squidSecondLine[ENEMIES_PER_LINE] = { 0 };

const int screenWidth = 1400;
const int screenHeight = 900;

void InitGame();
void ExecuteGameLoop();
void UpdateGame();
void RenderSpaceInvaders();
void MovePlayer(bool positive);
void HeroShoot();
void CheckEnemyCollision();
void MoveEnemies();

Hero hero;
Shoot heroShoot;
bool moveEnemiesToLeft = true;

int main() {
    InitWindow(screenWidth, screenHeight, "Space Invaders");
    InitGame();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        ExecuteGameLoop();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void InitGame()
{
    // Init hero
    hero.speed.x = -2.f;
    hero.rec.height = HERO_HEIGHT;
    hero.rec.width = HERO_WIDTH;
    hero.rec.x = (float)screenWidth / 2;
    hero.rec.y = (float)screenHeight - HERO_HEIGHT - 5;

    // Init hero shoot
    heroShoot.color = RED;
    heroShoot.rec.height = 20.f;
    heroShoot.rec.width = 5.f;
    heroShoot.speed.y = 7.f;
    heroShoot.active = false;

    int screenOffset = 100;
    int lineNumber = 0;

    // Init Octopus array
    Octopus defaultOctopus = {
        .active = true,
        .color = PINK,
        .speed = { .x = 1.f, .y = (float) ENEMY_HEIGHT / 2 },
        .rec = { .height = ENEMY_HEIGHT, .width = ENEMY_WIDTH },
        .value = 30
    };

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        octopus[i] = defaultOctopus;
        octopus[i].index = i;
        octopus[i].rec.x = ENEMY_WIDTH + screenOffset * ( i + 1);
    }
    lineNumber++;

    // Init Crab array
    Crab defaultCrab = {
        .active = true,
        .color = SKYBLUE,
        .speed = { .x = 1.f, .y = (float) ENEMY_HEIGHT / 2 },
        .rec = { .height = ENEMY_HEIGHT, .width = ENEMY_WIDTH },
        .value = 30
    };

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        crabFirstLine[i] = defaultCrab;
        crabFirstLine[i].index = i;
        crabFirstLine[i].rec.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        crabFirstLine[i].rec.y = ENEMY_HEIGHT * lineNumber * 2;
    }
    lineNumber++;

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        crabSecondLine[i] = defaultCrab;
        crabSecondLine[i].index = i;
        crabSecondLine[i].rec.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        crabSecondLine[i].rec.y = ENEMY_HEIGHT * lineNumber * 2;
    }
    lineNumber++;

    // Init Squid array
    Squid defaultSquid = {
        .active = true,
        .color = GREEN,
        .speed = { .x = 1.f, .y = (float) ENEMY_HEIGHT / 2 },
        .rec = { .height = ENEMY_HEIGHT, .width = ENEMY_WIDTH },
        .value = 30
    };

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        squidFirstLine[i] = defaultSquid;
        squidFirstLine[i].index = i;
        squidFirstLine[i].rec.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidFirstLine[i].rec.y = ENEMY_HEIGHT * lineNumber * 2;
    }
    lineNumber++;

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        squidSecondLine[i] = defaultSquid;
        squidSecondLine[i].index = i;
        squidSecondLine[i].rec.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidSecondLine[i].rec.y = ENEMY_HEIGHT * lineNumber * 2;
    }

    SetTargetFPS(60);
}

void ExecuteGameLoop()
{
    UpdateGame();
    RenderSpaceInvaders();
}

void UpdateGame()
{
    // Movement
    if (IsKeyDown(KEY_RIGHT)) MovePlayer(false);
    if (IsKeyDown(KEY_LEFT)) MovePlayer(true);

    // Actions
    if (IsKeyDown(KEY_SPACE) && !heroShoot.active) HeroShoot();

    // Shoot displacement
    if (heroShoot.active)
    {
        heroShoot.rec.y = heroShoot.rec.y - heroShoot.speed.y;

        // The user is able to shoot only when the previous shoot
        // collides with an enemy or "collides" with the end of the screen
        if (heroShoot.rec.y < 0) {
            heroShoot.active = false;
        } else
        {
            CheckEnemyCollision();
        }
    }

    MoveEnemies();
}

void RenderSpaceInvaders()
{
    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (octopus[i].active) DrawRectangleRec(octopus[i].rec, octopus[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (crabFirstLine[i].active) DrawRectangleRec(crabFirstLine[i].rec, crabFirstLine[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (crabSecondLine[i].active) DrawRectangleRec(crabSecondLine[i].rec, crabSecondLine[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (squidFirstLine[i].active) DrawRectangleRec(squidFirstLine[i].rec, squidFirstLine[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (squidSecondLine[i].active) DrawRectangleRec(squidSecondLine[i].rec, squidSecondLine[i].color);
    }

    if (heroShoot.active)
    {
        DrawRectangleRec(heroShoot.rec, heroShoot.color);
    }

    DrawRectangleRec(hero.rec, GREEN);
}

void MovePlayer(bool positive)
{
    hero.rec.x += positive ? hero.speed.x : -hero.speed.x;
}

void HeroShoot()
{
    if (!heroShoot.active)
    {
        heroShoot.active = true;
        heroShoot.rec.x = hero.rec.x + HERO_WIDTH / 2;
        heroShoot.rec.y = hero.rec.y - heroShoot.rec.height + 5;
    }
}

void CheckEnemyCollision()
{
    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (octopus[i].active)
            if (CheckCollisionRecs(heroShoot.rec, octopus[i].rec))
            {
                heroShoot.active = false;
                octopus[i].active = false;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (crabFirstLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, crabFirstLine[i].rec))
            {
                heroShoot.active = false;
                crabFirstLine[i].active = false;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (crabSecondLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, crabSecondLine[i].rec))
            {
                heroShoot.active = false;
                crabSecondLine[i].active = false;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (squidFirstLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, squidFirstLine[i].rec))
            {
                heroShoot.active = false;
                squidFirstLine[i].active = false;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (squidSecondLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, squidSecondLine[i].rec))
            {
                heroShoot.active = false;
                squidSecondLine[i].active = false;
            }
    }
}

void MoveEnemies()
{
    bool moveEnemiesInYAxis = false;
    if (octopus[9].rec.x >= screenWidth - 60)
    {
        moveEnemiesToLeft = false;
        moveEnemiesInYAxis = true;
    }
    if (octopus[0].rec.x <= 25)
    {
        moveEnemiesToLeft = true;
        moveEnemiesInYAxis = true;
    }

    const int xIncreaseFactor = 25;

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (!moveEnemiesInYAxis) octopus[i].rec.x += moveEnemiesToLeft ? octopus[i].speed.x : -octopus[i].speed.x;
        else
            {
            octopus[i].rec.y += octopus[i].speed.y;
            octopus[i].speed.x += octopus[i].speed.x / xIncreaseFactor;
            octopus[i].rec.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {

        if (!moveEnemiesInYAxis) crabFirstLine[i].rec.x += moveEnemiesToLeft ? crabFirstLine[i].speed.x : -crabFirstLine[i].speed.x;
        else
        {
            crabFirstLine[i].rec.y += crabFirstLine[i].speed.y;
            crabFirstLine[i].speed.x += crabFirstLine[i].speed.x / xIncreaseFactor;
            crabFirstLine[i].rec.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (!moveEnemiesInYAxis) crabSecondLine[i].rec.x += moveEnemiesToLeft ? crabSecondLine[i].speed.x : -crabSecondLine[i].speed.x;
        else
        {
            crabSecondLine[i].rec.y += crabSecondLine[i].speed.y;
            crabSecondLine[i].speed.x += crabSecondLine[i].speed.x / xIncreaseFactor;
            crabSecondLine[i].rec.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (!moveEnemiesInYAxis) squidFirstLine[i].rec.x += moveEnemiesToLeft ? squidFirstLine[i].speed.x : -squidFirstLine[i].speed.x;
        else
        {
            squidFirstLine[i].rec.y += squidFirstLine[i].speed.y;
            squidFirstLine[i].speed.x += squidFirstLine[i].speed.x / xIncreaseFactor;
            squidFirstLine[i].rec.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (!moveEnemiesInYAxis) squidSecondLine[i].rec.x += moveEnemiesToLeft ? squidSecondLine[i].speed.x : -squidSecondLine[i].speed.x;
        else
        {
            squidSecondLine[i].rec.y += squidSecondLine[i].speed.y;
            squidSecondLine[i].speed.x += squidSecondLine[i].speed.x / xIncreaseFactor;
            squidSecondLine[i].rec.x += moveEnemiesToLeft ? 1 : -1;
        }
    }
}