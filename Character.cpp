#include "Character.h"

Character::Character(const char *filename, int spriteWidth, int spriteHeight, int spriteRow, int spriteCount, int spritesPerSecond, float posX, float posY, float scale, bool isJumping, int velocity, bool onGround)
{
    texture = LoadTexture(filename);
    this->spriteWidth = spriteWidth;
    this->spriteHeight = spriteHeight;
    this->spriteRow = spriteRow;
    this->spriteCount = spriteCount;
    this->spritesPerSecond = spritesPerSecond;
    this->posX = posX;
    this->posY = posY;
    this->scale = scale;
    this->isJumping = isJumping;
    this->velocity = velocity;
    this->onGround = onGround;

    jumpSounds[0] = LoadSound("JumpSounds/sound1.wav");
    jumpSounds[1] = LoadSound("JumpSounds/sound2.wav");
    jumpSounds[2] = LoadSound("JumpSounds/sound3.wav");
    jumpSounds[3] = LoadSound("JumpSounds/sound4.wav");
    jumpSounds[4] = LoadSound("JumpSounds/sound5.wav");
    jumpSounds[5] = LoadSound("JumpSounds/sound6.wav");
    jumpSounds[6] = LoadSound("JumpSounds/sound7.wav");
}

void Character::move(float deltaTime, int windowWidth)
{
    float speed = 200.0f;

    bool leftInput = IsKeyDown(KEY_A);
    bool rightInput = IsKeyDown(KEY_D);

    if (GetTouchPointCount() > 0)
    {
        Vector2 touch = GetTouchPosition(0);

        if (touch.x < windowWidth / 2)
            leftInput = true;
        else
            rightInput = true;
    }

    if (rightInput && posX + spriteWidth * scale <= windowWidth + spriteWidth - border)
    {
        posX += speed * deltaTime;
        facingLeft = false;
    }

    if (leftInput && posX >= 0 - spriteWidth + border)
    {
        posX -= speed * deltaTime;
        facingLeft = true;
    }
}

void Character::fall(int windowHeight)
{
    posY -= velocity;

    if (posY + spriteHeight * scale < windowHeight)
    {
        velocity -= 1;
        onGround = false;
    }
    else
    {
        velocity = 0;
        posY = windowHeight - spriteHeight * scale;
        onGround = true;
    }
}

void Character::jump(float deltaTime, int windowHeight)
{
    bool jumpInput = IsKeyPressed(KEY_SPACE);

    if (GetTouchPointCount() > 0 && !isJumping && !onGround)
        jumpInput = true;

    if (jumpInput && !isJumping && !onGround)
    {
        int randomIndex = GetRandomValue(0, 5);
        PlaySound(jumpSounds[randomIndex]);

        velocity = 20;
        isJumping = true;
    }
}

void Character::draw()
{
    int spriteIndex = (int)(GetTime() * spritesPerSecond) % spriteCount;

    Rectangle spriteRec = {
        (float)spriteIndex * spriteWidth,
        (float)spriteRow * spriteHeight,
        facingLeft ? -(float)spriteWidth : (float)spriteWidth,
        (float)spriteHeight};

    Rectangle spriteDest = {
        posX,
        posY,
        (float)spriteWidth * scale,
        (float)spriteHeight * scale};

    Vector2 origin = {0, 0};

    DrawTexturePro(texture, spriteRec, spriteDest, origin, 0.0f, WHITE);
}

void Character::update(float deltaTime, bool isDead)
{
    bool leftInput = IsKeyDown(KEY_A);
    bool rightInput = IsKeyDown(KEY_D);

    if (GetTouchPointCount() > 0)
    {
        Vector2 touch = GetTouchPosition(0);

        if (touch.x < GetScreenWidth() / 2)
            leftInput = true;
        else
            rightInput = true;
    }

    if (!isJumping && (leftInput || rightInput))
    {
        spriteRow = 2;
        spriteCount = 4;
        spritesPerSecond = 3;
        facingLeft = leftInput;
        idleTimer = 0.0;
    }
    else if (isJumping && !isDead)
    {
        spriteRow = 5;
        spriteCount = 1;
        facingLeft = leftInput;
        idleTimer = 0.0;
    }
    else
    {
        spriteRow = 0;
        spriteCount = 1;
        idleTimer += deltaTime;

        if (idleTimer >= 3.0f)
        {
            spriteRow = 0;
            spriteCount = 26;
            spritesPerSecond = 5;
        }
    }
}

void Character::unload()
{
    UnloadTexture(texture);

    UnloadSound(jumpSounds[0]);
    UnloadSound(jumpSounds[1]);
    UnloadSound(jumpSounds[2]);
    UnloadSound(jumpSounds[3]);
    UnloadSound(jumpSounds[4]);
    UnloadSound(jumpSounds[5]);
    UnloadSound(jumpSounds[6]);
}
