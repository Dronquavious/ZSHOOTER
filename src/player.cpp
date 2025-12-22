#include "player.h"
#include <string>
#include <cmath>

Player::Player(float x, float y)
{
    position.x = x;
    position.y = y;
    speed = 200.0f;
    rotation = 0.0f;
    scale = 0.25f;

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


    //------ LOOKING AT MOUSE ROTATION LOGIC -----
    
    Vector2 mousePos = GetMousePosition();

    float dx = mousePos.x - position.x;
    float dy = mousePos.y - position.y;

    float angleRad = atan2(dy,dx);

    rotation = angleRad * RAD2DEG;

}

void Player::draw()
{
    Texture2D currentTexture = idleTextures[currentFrame];

    // the part of the image we want to use
    //    x: 0, y: 0, width: image width, height: image height
    Rectangle source = { 0, 0, (float)currentTexture.width, (float)currentTexture.height };

    // where on the screen it should be
    //    x: position.x, y: position.y
    //    width: image width * scale, height: image height * scale
    Rectangle dest = { position.x, position.y, currentTexture.width * scale, currentTexture.height * scale };

    // pivot points
    //    if we leave this at (0,0), the player will rotate around their top-left corner.
    //    We want the center, so we take half the destination width/height.
    Vector2 origin = { dest.width / 2, dest.height / 2 };

    DrawTexturePro(currentTexture, source, dest, origin, rotation, WHITE);
}