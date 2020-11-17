#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "raylib.h"

#define PLAYER_MAX_LIVES 3
#define HERO_HEIGHT      50
#define HERO_WIDTH       100
#define ENEMY_HEIGHT     40
#define ENEMY_WIDTH      40
#define ENEMIES_PER_LINE 10

typedef struct Hero {
    Rectangle rec;
    Vector2 speed;
    int score;
    int lifes;
} Hero;

typedef struct Shoot {
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
} Shoot;

typedef struct Invader {
    Texture2D sprite;
    Rectangle bounds;
    Vector2 pos;
    Vector2 speed;
    bool active;
    Color color;
    int value;
} Invader;

typedef struct EnemyShip {
    Rectangle rec;
    Vector2 speed;
    bool active;
    int value;
} EnemyShip;

Invader octopus[ENEMIES_PER_LINE] = { 0 };
Invader crabFirstLine[ENEMIES_PER_LINE] = { 0 };
Invader crabSecondLine[ENEMIES_PER_LINE] = { 0 };
Invader squidFirstLine[ENEMIES_PER_LINE] = { 0 };
Invader squidSecondLine[ENEMIES_PER_LINE] = { 0 };

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
    hero.rec.y = (float)screenHeight - HERO_HEIGHT * 2 - 50;
    hero.score = 0;
    hero.lifes = PLAYER_MAX_LIVES;

    // Init hero shoot
    heroShoot.color = RED;
    heroShoot.rec.height = 20.f;
    heroShoot.rec.width = 5.f;
    heroShoot.speed.y = 7.f;
    heroShoot.active = false;

    int screenOffset = 100;
    int lineNumber = 0;

    Texture2D octopusTexture = LoadTexture("resources/Octopus.png");

    // Init Octopus array
    Invader defaultOctopus = {
        .active = true,
        .color = PINK,
        .speed = { .x = 1.f, .y = (float) ENEMY_HEIGHT / 2 },
        .sprite = octopusTexture,
        .bounds = { .height = 65, .width = 75 },
        .value = 30
    };

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        octopus[i] = defaultOctopus;
        octopus[i].pos.x = ENEMY_WIDTH + screenOffset * (i + 1);
        octopus[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
    }
    lineNumber++;

    Texture2D crabTexture = LoadTexture("resources/Crab.png");

    // Init Crab array
    Invader defaultCrab = {
        .active = true,
        .color = SKYBLUE,
        .speed = { .x = 1.f, .y = (float) ENEMY_HEIGHT / 2 },
        .sprite = crabTexture,
        .bounds = { .height = 65, .width = 75 },
        .value = 20
    };

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        crabFirstLine[i] = defaultCrab;
        crabFirstLine[i].pos.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        crabFirstLine[i].pos.y = ENEMY_HEIGHT * lineNumber * 2;
        crabFirstLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        crabFirstLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 2;
    }
    lineNumber++;

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        crabSecondLine[i] = defaultCrab;
        crabSecondLine[i].pos.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        crabSecondLine[i].pos.y = ENEMY_HEIGHT * lineNumber * 2;
        crabSecondLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        crabSecondLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 2;
    }
    lineNumber++;

    Texture2D squidTexture = LoadTexture("resources/Squid.png");

    // Init Squid array
    Invader defaultSquid = {
        .active = true,
        .color = GREEN,
        .speed = { .x = 1.f, .y = (float) ENEMY_HEIGHT / 2 },
        .sprite = squidTexture,
        .bounds = { .height = 65, .width = 75 },
        .value = 10
    };

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        squidFirstLine[i] = defaultSquid;
        squidFirstLine[i].pos.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidFirstLine[i].pos.y = ENEMY_HEIGHT * lineNumber * 2;
        squidFirstLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidFirstLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 2;
    }
    lineNumber++;

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        squidSecondLine[i] = defaultSquid;
        squidSecondLine[i].pos.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidSecondLine[i].pos.y = ENEMY_HEIGHT * lineNumber * 2;
        squidSecondLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidSecondLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 2;
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

void RenderSpaceInvaders(int algo)
{
    DrawText(TextFormat("Score: %d", hero.score), 10, 25, 12, GREEN);
    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (octopus[i].active) DrawTexture(octopus[i].sprite, octopus[i].bounds.x, octopus[i].bounds.y , octopus[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (crabFirstLine[i].active) DrawTexture(crabFirstLine[i].sprite, crabFirstLine[i].bounds.x, crabFirstLine[i].bounds.y, crabFirstLine[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (crabSecondLine[i].active) DrawTexture(crabSecondLine[i].sprite, crabSecondLine[i].bounds.x, crabSecondLine[i].bounds.y, crabSecondLine[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (squidFirstLine[i].active) DrawTexture(squidFirstLine[i].sprite, squidFirstLine[i].bounds.x, squidFirstLine[i].bounds.y, squidFirstLine[i].color);
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (squidSecondLine[i].active) DrawTexture(squidSecondLine[i].sprite, squidSecondLine[i].bounds.x, squidSecondLine[i].bounds.y, squidSecondLine[i].color);
    }

    if (heroShoot.active)
    {
        DrawRectangleRec(heroShoot.rec, heroShoot.color);
    }

    DrawRectangleRec(hero.rec, GREEN);
    for(int i = 0; i < hero.lifes; i++)
    {
        DrawRectangle(i * 100 + (i * 10) + 10, GetScreenHeight() - HERO_HEIGHT - 10, HERO_WIDTH, HERO_HEIGHT, GREEN);
    }
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
            if (CheckCollisionRecs(heroShoot.rec, octopus[i].bounds))
            {
                heroShoot.active = false;
                octopus[i].active = false;
                hero.score += octopus[i].value;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (crabFirstLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, crabFirstLine[i].bounds))
            {
                heroShoot.active = false;
                crabFirstLine[i].active = false;
                hero.score += crabFirstLine[i].value;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (crabSecondLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, crabSecondLine[i].bounds))
            {
                heroShoot.active = false;
                crabSecondLine[i].active = false;
                hero.score += crabSecondLine[i].value;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (squidFirstLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, squidFirstLine[i].bounds))
            {
                heroShoot.active = false;
                squidFirstLine[i].active = false;
                hero.score += squidFirstLine[i].value;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (squidSecondLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, squidSecondLine[i].bounds))
            {
                heroShoot.active = false;
                squidSecondLine[i].active = false;
                hero.score += squidSecondLine[i].value;
            }
    }
}

void MoveEnemies()
{
    bool moveEnemiesInYAxis = false;
    if (octopus[9].bounds.x >= screenWidth - 125)
    {
        moveEnemiesToLeft = false;
        moveEnemiesInYAxis = true;
    }
    if (octopus[0].bounds.x <= 25)
    {
        moveEnemiesToLeft = true;
        moveEnemiesInYAxis = true;
    }

    const int xIncreaseFactor = 18.5;

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (!moveEnemiesInYAxis) octopus[i].bounds.x += moveEnemiesToLeft ? octopus[i].speed.x : -octopus[i].speed.x;
        else
            {
            octopus[i].bounds.y += octopus[i].speed.y;
            octopus[i].speed.x += octopus[i].speed.x / xIncreaseFactor;
            octopus[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {

        if (!moveEnemiesInYAxis) crabFirstLine[i].bounds.x += moveEnemiesToLeft ? crabFirstLine[i].speed.x : -crabFirstLine[i].speed.x;
        else
        {
            crabFirstLine[i].bounds.y += crabFirstLine[i].speed.y;
            crabFirstLine[i].speed.x += crabFirstLine[i].speed.x / xIncreaseFactor;
            crabFirstLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (!moveEnemiesInYAxis) crabSecondLine[i].bounds.x += moveEnemiesToLeft ? crabSecondLine[i].speed.x : -crabSecondLine[i].speed.x;
        else
        {
            crabSecondLine[i].bounds.y += crabSecondLine[i].speed.y;
            crabSecondLine[i].speed.x += crabSecondLine[i].speed.x / xIncreaseFactor;
            crabSecondLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (!moveEnemiesInYAxis) squidFirstLine[i].bounds.x += moveEnemiesToLeft ? squidFirstLine[i].speed.x : -squidFirstLine[i].speed.x;
        else
        {
            squidFirstLine[i].bounds.y += squidFirstLine[i].speed.y;
            squidFirstLine[i].speed.x += squidFirstLine[i].speed.x / xIncreaseFactor;
            squidFirstLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; ++i) {
        if (!moveEnemiesInYAxis) squidSecondLine[i].bounds.x += moveEnemiesToLeft ? squidSecondLine[i].speed.x : -squidSecondLine[i].speed.x;
        else
        {
            squidSecondLine[i].bounds.y += squidSecondLine[i].speed.y;
            squidSecondLine[i].speed.x += squidSecondLine[i].speed.x / xIncreaseFactor;
            squidSecondLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }
}