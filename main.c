#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "raylib.h"

#define PLAYER_MAX_LIVES 3
#define HERO_HEIGHT      50
#define HERO_WIDTH       100
#define ENEMY_HEIGHT     32
#define ENEMY_WIDTH      32
#define ENEMIES_PER_LINE 10
#define ENEMY_SHOOT_TIME 60

typedef struct Hero {
    Rectangle rec;
    Vector2 speed;
    int score;
    int lives;
} Hero;

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

typedef struct Invader {
    Texture2D sprite;
    Rectangle bounds;
    Vector2 pos;
    Vector2 speed;
    bool active;
    Color color;
    int value;
} Invader;

typedef struct InvaderShoot {
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
} InvaderShoot;

typedef struct EnemyShip {
    Rectangle rec;
    Vector2 speed;
    bool active;
    int value;
} EnemyShip;

typedef struct MenuOption {
    char *text;
    Vector2 pos;
} MenuOption;

Invader octopus[ENEMIES_PER_LINE] = { 0 };
Invader crabFirstLine[ENEMIES_PER_LINE] = { 0 };
Invader crabSecondLine[ENEMIES_PER_LINE] = { 0 };
Invader squidFirstLine[ENEMIES_PER_LINE] = { 0 };
Invader squidSecondLine[ENEMIES_PER_LINE] = { 0 };
Defense defenses[3];

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
void ResumeGame();
void WordBox();
Hero hero;
Shoot heroShoot;
InvaderShoot invaderShoot;


bool moveEnemiesToLeft = true;
bool pause = false;
bool isUserDeath = false;
bool menu = false;
bool count = false;

int add = 0;
int frame = 0;

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
    hero.lives = PLAYER_MAX_LIVES;

    // Init hero shoot
    heroShoot.color = RED;
    heroShoot.rec.height = 20.f;
    heroShoot.rec.width = 5.f;
    heroShoot.speed.y = 7.f;
    heroShoot.active = false;

    // Init enemy shoot
    invaderShoot.rec.height = 20.f;
    invaderShoot.rec.width = 5.f;
    invaderShoot.color = BLACK;
    invaderShoot.speed.y = 7.f;

    int screenOffset = 75;
    int lineNumber = 1;

    Texture2D octopusTexture = LoadTexture("resources/Octopus.png");

    // Init Octopus array
    Invader defaultOctopus = {
        .active = true,
        .color = PINK,
        .speed = { .x = 1.5f, .y = (float) ENEMY_HEIGHT / 2 },
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

    Texture2D crabTexture = LoadTexture("resources/Crab.png");

    // Init Crab array
    Invader defaultCrab = {
        .active = true,
        .color = SKYBLUE,
        .speed = { .x = 1.5f, .y = (float) ENEMY_HEIGHT / 2 },
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

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        crabSecondLine[i] = defaultCrab;
        crabSecondLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        crabSecondLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 1.4;
    }
    lineNumber++;

    Texture2D squidTexture = LoadTexture("resources/Squid.png");

    // Init Squid array
    Invader defaultSquid = {
        .active = true,
        .color = GREEN,
        .speed = { .x = 1.5f, .y = (float) ENEMY_HEIGHT / 2 },
        .sprite = squidTexture,
        .bounds = { .height = 32, .width = 48 },
        .value = 10
    };

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        squidFirstLine[i] = defaultSquid;
        squidFirstLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidFirstLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 1.4;
    }
    lineNumber++;

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        squidSecondLine[i] = defaultSquid;
        squidSecondLine[i].bounds.x = ENEMY_WIDTH + screenOffset * ( i + 1);
        squidSecondLine[i].bounds.y = ENEMY_HEIGHT * lineNumber * 1.5;
    }

    // Init Defenses
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

    SetTargetFPS(60);
}

void ExecuteGameLoop()
{
    UpdateGame();
    RenderSpaceInvaders();
}

void UpdateGame()
{
    // Check if it is pause or menu
    if (IsKeyPressed(KEY_P)) pause = !pause;
    if (IsKeyPressed(KEY_M)) menu = !menu;

    // Pause text
    if (pause) DrawText(TextFormat("GAME PAUSED"), screenWidth / 2 - 150, screenHeight - 50, 40, DARKBLUE);

    if (!pause && !menu && !isUserDeath) {
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
            } else {
                CheckEnemyCollision();
            }
        }

        if (invaderShoot.active)
        {
            invaderShoot.rec.y += invaderShoot.speed.y;

            if (invaderShoot.rec.y >= (double)screenHeight) {
                invaderShoot.active = false;
                invaderShoot.rec.y = 0;
            } else {
                CheckHeroCollision();
            }
        }

        CheckDefenseCollision();
        EnemyShoot();
        MoveEnemies();
    }
    if (isUserDeath)
    {
        DrawText("Presiona \nbarra espaciadora \npara continuar", screenHeight / 4, screenWidth / 4, 100, GREEN);
        if (hero.lives > 0 && IsKeyDown(KEY_SPACE)) isUserDeath = false;
    }
}

void RenderSpaceInvaders()
{
    DrawText(TextFormat("Score: %d", hero.score), 10, 5, 24, GREEN);
    DrawText(TextFormat("PRESS M TO GO TO MENU"), screenWidth-300, screenHeight-30, 20, GRAY);
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

    if (heroShoot.active)
    {
        DrawRectangleRec(heroShoot.rec, heroShoot.color);
    }

    if (invaderShoot.active)
    {
        DrawRectangleRec(invaderShoot.rec, invaderShoot.color);
    }

    RenderDefenses();

    DrawRectangleRec(hero.rec, GREEN);
    for(int i = 0; i < hero.lives - 1; i++)
    {
        DrawRectangle(i * 100 + (i * 10) + 10, GetScreenHeight() - HERO_HEIGHT - 10, HERO_WIDTH, HERO_HEIGHT, GREEN);
    }

    RenderMenu();
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
        heroShoot.rec.x = hero.rec.x + HERO_WIDTH / 2.f;
        heroShoot.rec.y = hero.rec.y - heroShoot.rec.height + 5;
    }
}

void CheckEnemyCollision()
{
    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (octopus[i].active)
            if (CheckCollisionRecs(heroShoot.rec, octopus[i].bounds))
            {
                heroShoot.active = false;
                octopus[i].active = false;
                hero.score += octopus[i].value;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (crabFirstLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, crabFirstLine[i].bounds))
            {
                heroShoot.active = false;
                crabFirstLine[i].active = false;
                hero.score += crabFirstLine[i].value;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (crabSecondLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, crabSecondLine[i].bounds))
            {
                heroShoot.active = false;
                crabSecondLine[i].active = false;
                hero.score += crabSecondLine[i].value;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (squidFirstLine[i].active)
            if (CheckCollisionRecs(heroShoot.rec, squidFirstLine[i].bounds))
            {
                heroShoot.active = false;
                squidFirstLine[i].active = false;
                hero.score += squidFirstLine[i].value;
            }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
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
    for(int i = 9; i >= 0; i--)
    {
        if (octopus[i].active)
        {
            if (octopus[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (crabFirstLine[i].active)
        {
            if (crabFirstLine[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (crabSecondLine[i].active)
        {
            if (crabSecondLine[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (squidFirstLine[i].active)
        {
            if (squidFirstLine[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
        if (squidSecondLine[i].active)
        {
            if (squidSecondLine[i].bounds.x >= (float) screenWidth - 50)
            {
                moveEnemiesToLeft = false;
                moveEnemiesInYAxis = true;
            }
            break;
        }
    }
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

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (!moveEnemiesInYAxis) octopus[i].bounds.x += moveEnemiesToLeft ? octopus[i].speed.x : -octopus[i].speed.x;
        else
            {
            octopus[i].bounds.y += octopus[i].speed.y;
            octopus[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {

        if (!moveEnemiesInYAxis) crabFirstLine[i].bounds.x += moveEnemiesToLeft ? crabFirstLine[i].speed.x : -crabFirstLine[i].speed.x;
        else
        {
            crabFirstLine[i].bounds.y += crabFirstLine[i].speed.y;
            crabFirstLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (!moveEnemiesInYAxis) crabSecondLine[i].bounds.x += moveEnemiesToLeft ? crabSecondLine[i].speed.x : -crabSecondLine[i].speed.x;
        else
        {
            crabSecondLine[i].bounds.y += crabSecondLine[i].speed.y;
            crabSecondLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (!moveEnemiesInYAxis) squidFirstLine[i].bounds.x += moveEnemiesToLeft ? squidFirstLine[i].speed.x : -squidFirstLine[i].speed.x;
        else
        {
            squidFirstLine[i].bounds.y += squidFirstLine[i].speed.y;
            squidFirstLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }

    for (int i = 0; i < ENEMIES_PER_LINE; i++) {
        if (!moveEnemiesInYAxis) squidSecondLine[i].bounds.x += moveEnemiesToLeft ? squidSecondLine[i].speed.x : -squidSecondLine[i].speed.x;
        else
        {
            squidSecondLine[i].bounds.y += squidSecondLine[i].speed.y;
            squidSecondLine[i].bounds.x += moveEnemiesToLeft ? 1 : -1;
        }
    }
}

void EnemyShoot()
{
    if (!invaderShoot.active)
    {
        frame++;
        if (frame >= ENEMY_SHOOT_TIME)
        {
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

                if (enemy.active && enemy.bounds.x >= hero.rec.x - (HERO_WIDTH / 2.f) && enemy.bounds.x <= hero.rec.x + (HERO_WIDTH / 2.f))
                {
                    invaderShoot.rec.y = enemy.bounds.y + enemy.bounds.height;
                    invaderShoot.rec.x = enemy.bounds.x + (enemy.bounds.width / 2.f);
                    invaderShoot.active = true;
                }
            }
        }
    } else {
        frame = 0;
    }
}

void CheckHeroCollision()
{
    if (invaderShoot.active)
    {
        if (CheckCollisionRecs(invaderShoot.rec, hero.rec))
        {
            invaderShoot.active = false;
            hero.lives--;
            isUserDeath = true;
        }
    }
}

void CheckDefenseCollision()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 6; j++) {
            if (defenses[i].active[j])
            {
                if (heroShoot.active)
                {
                    if (CheckCollisionRecs(heroShoot.rec, defenses[i].structure[j]))
                    {
                        heroShoot.active = false;
                        defenses[i].active[j] = false;
                    }
                }
                if (invaderShoot.active)
                {
                    if (CheckCollisionRecs(invaderShoot.rec, defenses[i].structure[j]))
                    {
                        invaderShoot.active = false;
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
        DrawRectangleRec(hero.rec, GREEN);

        char *menuLetters[] = {"-> SAVE GAME", "-> LOAD GAME", "-> RESUME"};
        MenuOption menuOp[strlen((const char *) menuLetters)];
        int pos = (screenHeight/2)-125+(add*40);

        DrawRectangle((screenWidth/2)-200, (screenHeight/2)-135, 400, 270, BLACK);
        DrawRectangle((screenWidth/2)-180, (screenHeight/2)-125+(add*40), 350, 40, GRAY);

        if(IsKeyReleased((KEY_UP))) add--;
        if(IsKeyReleased((KEY_DOWN))) add++;
        printf("add: %d", add);

        for(int i=0; i<strlen((const char *) menuLetters); i++) {
            menuOp[i].text = menuLetters[i];
            menuOp[i].pos.x = (screenWidth/2)-180;
            menuOp[i].pos.y = (screenHeight/2)-125+(i*40);
            DrawText(menuOp[i].text, menuOp[i].pos.x, menuOp[i].pos.y, 40, WHITE);
        }


        if((pos == ((screenHeight/2)-125+(0*40))) && (IsKeyDown(KEY_ENTER))) SaveGame();
        if((pos == ((screenHeight/2)-125+(1*40))) && (IsKeyDown(KEY_ENTER))) LoadGame();
        if((pos == ((screenHeight/2)-125+(2*40))) && (IsKeyDown(KEY_ENTER))) ResumeGame();



/*
        DrawRectangle(500, 315, 400, 270, BLACK);
        DrawRectangle(510, 325+add, 350, 60, GRAY);
        DrawText(TextFormat("-> SAVE GAME"), 520, 335, 40, WHITE);
        DrawText(TextFormat("-> LOAD GAME"), 520, 415, 40, WHITE);
        DrawText(TextFormat("-> RESUME"), 520, 495, 40, WHITE);
        //DrawText(TextFormat("PRESS M TO RESUME"), 520,730 , 30, WHITE);

        // Select resume in menu
        if((325+add >= 405) && (IsKeyDown(KEY_ENTER))) {
            //SaveGame();
            LoadGame();
        }*/
    }
}

void SaveGame()
{
    Invader gameToSave[50];
    int j = 0;
    for (int i = 0; i < 5; i++)
    {
        Invader *enemy;
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
            gameToSave[j] = enemy[ref];
            ref++;
        }
    }

    FILE *fileW = fopen("Enemies.bin", "wb");
    fwrite(gameToSave, sizeof(struct Invader), 50, fileW);
    fwrite(&invaderShoot, sizeof(struct InvaderShoot), 1, fileW);
    fwrite(&hero, sizeof(struct Hero), 1, fileW);
    fwrite(&heroShoot, sizeof(struct Shoot), 1, fileW);
    fwrite(&defenses, sizeof(struct Defense), 3, fileW);
    fclose(fileW);
}

void LoadGame()
{
    Invader enemies[50];
    InvaderShoot invShoot;
    Hero hero1;
    Shoot hShoot;
    Defense def[3];
    FILE *file = fopen("Enemies.bin", "rb");
    if (file != NULL)
    {
        for (int i = 0; i < 50; ++i)
        {
            fread(&enemies[i], sizeof(struct Invader), 1, file);
        }

        fread(&invShoot, sizeof(struct InvaderShoot), 1, file);
        invaderShoot = invShoot;

        int j = 0;
        for (int i = 0; i < 50; i++) {
            if (j == 10) j = 0;
            if (i < 10) octopus[j] = enemies[i];
            else if (i < 20) squidFirstLine[j] = enemies[i];
            else if (i < 30) squidSecondLine[j] = enemies[i];
            else if (i < 40) crabFirstLine[j] = enemies[i];
            else crabSecondLine[j] = enemies[i];
            j++;
        }

        fread(&hero1, sizeof(struct Hero), 1, file);
        hero = hero1;

        fread(&hShoot, sizeof(struct Shoot), 1, file);
        heroShoot = hShoot;

        for (int i = 0; i < 3; ++i) {
            fread(&def[i], sizeof(struct Defense), 1, file);
            defenses[i] = def[i];
        }

        fclose(file);
    }
}

void ResumeGame() {
    menu = !menu;
}

void WordBox() {

}