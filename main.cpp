#include "raylib.h"
#include "Character.h"
#include "Platform.h"
#include "Background.h"

const int windowWidth = 540;
const int windowHeight = 960;

float deathStartTime = 0;
bool isDead = false;
int deaths = 0;
bool isPaused = false;
float scrollSpeed = 3.0f;
float lastSpeedIncrease = 0;
bool gameStarted = false;

int score = 0;
int highscore = 0;
float scoreTimer = 0;

void death(Character &player, Platform &startPlatform)
{
    if (!isDead)
    {
        deathStartTime = GetTime();
        isDead = true;

        if (score > highscore)
            highscore = score;
    }

    DrawText("Tot!", windowWidth / 2 - 100, windowHeight / 2, 100, RED);

    int deathTimer = 1 + (int)(GetTime() - deathStartTime);

    if (deathTimer >= 5)
    {
        DrawText("Restart? Press R", player.border + 5, windowHeight / 2 - 100, 45, RED);

        if (IsKeyPressed(KEY_R))
        {
            player.posY = windowHeight / 2 - 50;
            player.posX = windowWidth / 2;

            startPlatform.posX = player.posX - startPlatform.width / 2;
            startPlatform.posY = player.posY + player.spriteHeight * player.scale + 50;

            deathStartTime = 0;
            isDead = false;

            score = 0;
            scoreTimer = 0;

            deaths += 1;
        }
    }
}

void LandOnPlatform(Character &player, Rectangle platform)
{
    Rectangle playerCollisionRec =
    {
        player.posX + 20,
        player.posY + player.spriteHeight * player.scale - 10,
        player.spriteWidth * player.scale - 40,
        10
    };

    if (CheckCollisionRecs(playerCollisionRec, platform))
    {
        bool onPlatform = (player.posY + player.spriteHeight * player.scale + player.velocity) <= platform.y;

        if (onPlatform)
        {
            player.isJumping = false;
            player.velocity = 0;
            player.posY = platform.y - player.spriteHeight * player.scale;
        }
    }
}

void BackgroundScroll(Character &player, Background &background, Platform *platforms[], int numPlatforms, Platform &startPlatform)
{
    if (!isDead && !isPaused)
    {
        if (GetTime() - lastSpeedIncrease >= 5.0f)
        {
            scrollSpeed += 0.5f;
            lastSpeedIncrease = GetTime();
        }
    }
    else if (isDead && background.bgY1 > 15.0f)
    {
        scrollSpeed = -15.0f;
    }
    else
    {
        scrollSpeed = 0.0f;

        if (background.bgY1 < 15.0f)
            background.bgY1 = 15.0f;
    }

    background.bgY1 += scrollSpeed;

    if (background.bgY1 >= windowHeight)
    {
        background.bgY1 = 0;
    }

    for (int i = 0; i < numPlatforms; i++)
    {
        platforms[i]->posY += scrollSpeed;

        if (platforms[i]->posY > windowHeight)
        {
            float highestY = platforms[0]->posY;

            for (int j = 1; j < numPlatforms; j++)
            {
                if (platforms[j]->posY < highestY)
                {
                    highestY = platforms[j]->posY;
                }
            }

            platforms[i]->posY = highestY - GetRandomValue(120, 180);

            platforms[i]->posX = GetRandomValue(
                player.border + 30,
                windowWidth - player.border - platforms[i]->width - 30);
        }
    }

    startPlatform.posY += scrollSpeed;

    if (startPlatform.posY > windowHeight)
    {
        startPlatform.posY = -200;
        startPlatform.posX = windowWidth / 2;
    }
}

int main()
{
    InitWindow(windowWidth, windowHeight, "JumpJump");
    InitAudioDevice();

    lastSpeedIncrease = GetTime();

    Character player(
        "Character/character.png",
        16,
        16,
        0,
        1,
        1,
        windowWidth / 2,
        windowHeight / 2 - 13,
        6.0f,
        false,
        0,
        false);

    Platform startPlatform(
        "Pads/PNG/Pad_1_1.png",
        windowWidth / 2,
        windowHeight / 2 + 87,
        0.25f,
        394 * 0.25f);

    Platform *platforms[10];

    for (int i = 0; i < 10; i++)
    {
        float x = GetRandomValue(
            player.border + 30,
            windowWidth - player.border - startPlatform.width - 30);

        float y = windowHeight / 2 - i * 200;

        platforms[i] = new Platform(
            "Pads/PNG/Pad_1_1.png",
            x,
            y,
            0.25f,
            startPlatform.width);
    }

    Background background(
        "Background/PNG/Game_Background_1/Layers/sky.png",
        "Background/PNG/Game_Background_1/Layers/sides.png",
        "Background/PNG/Game_Background_1/Layers/Ground.png",
        "Background/PNG/Game_Background_1/Layers/Decor.png",
        "Background/PNG/Game_Background_1/Layers/Clouds.png",
        "Background/PNG/Game_Background_1/Layers/BackGround.png",
        0.0f);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (!isDead && !isPaused && gameStarted)
        {
            scoreTimer += deltaTime;

            if (scoreTimer >= 1.0f)
            {
                score += 1;
                scoreTimer = 0;
            }
        }

        if (!gameStarted)
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                gameStarted = true;
                lastSpeedIncrease = GetTime();
            }
        }

        if (gameStarted)
        {
            player.update(deltaTime, isDead);
            player.move(deltaTime, windowWidth);
            player.jump(deltaTime, windowHeight);
            player.fall(windowHeight);

            BackgroundScroll(player, background, platforms, 10, startPlatform);
        }

        BeginDrawing();
        ClearBackground(WHITE);

        if (!gameStarted)
        {
            DrawText("JumpJump", windowWidth / 2 - 180, windowHeight / 2 - 100, 80, BLACK);
            DrawText("Press ENTER to start", windowWidth / 2 - 240, windowHeight / 2 + 20, 40, DARKGRAY);
        }
        else
        {
            background.draw(windowHeight);

            startPlatform.draw();

            for (int i = 0; i < 10; i++)
            {
                platforms[i]->draw();

                float x = platforms[i]->posX;
                float y = platforms[i]->posY;

                float width = platforms[i]->texture.width * platforms[i]->scale;
                float height = platforms[i]->texture.height * platforms[i]->scale;

                Rectangle rec = {x, y, width, height};

                LandOnPlatform(player, rec);
            }

            Rectangle startPlatformCollision =
            {
                startPlatform.posX,
                startPlatform.posY,
                startPlatform.texture.width * startPlatform.scale,
                startPlatform.texture.height * startPlatform.scale
            };

            LandOnPlatform(player, startPlatformCollision);

            if (player.velocity < -30 || player.onGround)
            {
                death(player, startPlatform);
            }

            if (IsKeyPressed(KEY_P))
            {
                isPaused = !isPaused;
            }

            player.draw();

            background.drawDecor(windowHeight);

            if (isPaused)
                DrawText("Pause", 125, 400, 100, GREEN);

            DrawText(TextFormat("Score: %i", score), 20, 30, 30, BLACK);
            DrawText(TextFormat("Highscore: %i", highscore), 20, 65, 30, BLACK);
            DrawText(TextFormat("deaths: %02i", deaths), 20, 100, 20, GRAY);
        }

        EndDrawing();
    }

    background.unload();
    player.unload();
    startPlatform.unload();

    CloseWindow();
}
