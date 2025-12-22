#include "player.h"
#include <string>

Player::Player(float x, float y)
{
    position.x = x;
    position.y = y;
    speed = 200.0f;
    rotation = 0.0f;

    currentFrame = 0;
    frameTimer = 0.0f;
    frameSpeed = 0.05f;

    for (int i = 0; i < 20; i++)
    {

        std::string fileName = "assets/playerRifle/idle/survivor-idle_rifle_" + std::to_string(i) +
                               ".png";

        Texture2D texture = LoadTexture(fileName.c_str());
        idleTextures.push_back(texture);
    }
}

Player::~Player()
{
    // unloading texttures
    for (Texture2D texture : idleTextures)
    {
        UnloadTexture(texture);
    }
}

void Player::update()
{
    float deltaTime = GetFrameTime();

    // movement
    if (IsKeyDown(KEY_W))
        position.y -= speed * deltaTime; // Move up
    if (IsKeyDown(KEY_S))
        position.y += speed * deltaTime; // Move down
    if (IsKeyDown(KEY_A))
        position.x -= speed * deltaTime; // Move left
    if (IsKeyDown(KEY_D))
        position.x += speed * deltaTime; // Move right

    frameTimer += deltaTime;

    if (frameTimer >= frameSpeed){
        
        frameTimer = 0.0f;
        currentFrame++;

        // loop back
        if (currentFrame >= idleTextures.size()) 
        {
            currentFrame = 0;
        }
    }
}

void Player::draw()
{
    DrawTexture(idleTextures[currentFrame],(int)position.x, (int)position.y, WHITE);
}