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
    isShooting = false;
    maxAmmo = 30;
    currentAmmo = 30;
    isReloading = false;
    health = 100;

    currentFrame = 0;
    frameTimer = 0.0f;
    frameSpeed = 0.05f;

    // idle textures
    for (int i = 0; i < 20; i++)
    {

        std::string fileName = "assets/playerRifle/idle/survivor-idle_rifle_" + std::to_string(i) +
                               ".png";

        Texture2D texture = LoadTexture(fileName.c_str());
        idleTextures.push_back(texture);
    }

    // shooting textures
    for (int i = 0; i < 3; i++)
    {

        std::string fileName = "assets/playerRifle/shoot/survivor-shoot_rifle_" + std::to_string(i) +
                               ".png";

        Texture2D texture = LoadTexture(fileName.c_str());
        shootTextures.push_back(texture);
    }

    // reload
    for (int i = 0; i < 20; i++)
    {

        std::string fileName = "assets/playerRifle/reload/survivor-reload_rifle_" + std::to_string(i) +
                               ".png";

        Texture2D texture = LoadTexture(fileName.c_str());
        reloadTextures.push_back(texture);
    }
}

Player::~Player()
{
    // unloading texttures
    for (Texture2D texture : idleTextures)
    {
        UnloadTexture(texture);
    }

    for (Texture2D texture : shootTextures)
    {
        UnloadTexture(texture);
    }

    for (Texture2D texture : reloadTextures)
    {
        UnloadTexture(texture);
    }
}

void Player::shoot()
{
    if (!isShooting)
    {
        isShooting = true;
        currentFrame = 0; // start shoot anim from beginning
        frameTimer = 0.0f;
        currentAmmo--;
    }
}

void Player::update(Vector2 mousePos)
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

    if (IsKeyPressed(KEY_R))
        reload(); // reload input

    // -- -ANIMATION STATE MACHINE-- -
    frameTimer += deltaTime;

    // which animation list to use
    std::vector<Texture2D> *currentAnim = &idleTextures; // default to idle

    if (isReloading)
    {
        currentAnim = &reloadTextures;
        frameSpeed = 0.1f; // SLOWER: 1 frame every 0.15 seconds
    }
    else if (isShooting)
    {
        currentAnim = &shootTextures;
        frameSpeed = 0.05f; // FASTER: 1 frame every 0.05 seconds
    }
    else
    {
        currentAnim = &idleTextures; // idle
        frameSpeed = 0.1f;           // nORMAL
    }

    // advance Frame
    if (frameTimer >= frameSpeed)
    {
        frameTimer = 0.0f;
        currentFrame++;

        // loop or end logic
        if (currentFrame >= currentAnim->size())
        {
            // RELOAD FINISHED
            if (isReloading)
            {
                isReloading = false;
                currentAmmo = maxAmmo; // refill bullets!
                currentFrame = 0;
            }
            // SHOOT FINISHED
            else if (isShooting)
            {
                isShooting = false;
                currentFrame = 0;
            }
            // IDLE LOOP
            else
            {
                currentFrame = 0;
            }
        }
    }

    //------ LOOKING AT MOUSE ROTATION LOGIC -----

    float dx = mousePos.x - position.x;
    float dy = mousePos.y - position.y;

    float angleRad = atan2(dy, dx);

    rotation = angleRad * RAD2DEG;
}

void Player::draw()
{

    // PICK THE CORRECT ANIMATION LIST
    std::vector<Texture2D>* currentAnim = &idleTextures; // default

    if (isReloading)
    {
        currentAnim = &reloadTextures;
    }
    else if (isShooting)
    {
        currentAnim = &shootTextures;
    }

    // safety Check: Ensure currentFrame isn't out of bounds 
    // (This prevents crashing if switching animations mid-frame)
    if (currentFrame >= currentAnim->size()) currentFrame = 0;

    Texture2D currentTexture = (*currentAnim)[currentFrame];

    // the part of the image we want to use
    //    x: 0, y: 0, width: image width, height: image height
    Rectangle source = {0, 0, (float)currentTexture.width, (float)currentTexture.height};

    // where on the screen it should be
    //    x: position.x, y: position.y
    //    width: image width * scale, height: image height * scale
    Rectangle dest = {position.x, position.y, currentTexture.width * scale, currentTexture.height * scale};

    // pivot points
    //    if we leave this at (0,0), the player will rotate around their top-left corner.
    //    We want the center, so we take half the destination width/height.
    Vector2 origin = {dest.width / 2, dest.height / 2};

    DrawTexturePro(currentTexture, source, dest, origin, rotation, WHITE);
}

Vector2 Player::getPlayerPos()
{
    return {position.x, position.y};
}

float Player::getPlayerRotation()
{
    return rotation;
}

void Player::reload()
{
    if (!isReloading && currentAmmo < maxAmmo)
    {
        isReloading = true;
        currentFrame = 0;
        frameTimer = 0.0f;
    }
}

bool Player::isReloadingState()
{
    return isReloading;
}

int Player::getAmmo()
{
    return currentAmmo;
}

int Player::getHealth() 
{ 
    return health; 
}

void Player::takeDamage(int damage)
{
    health -= damage;
}

void Player::reset()
{
    position = {100.0f, 100.0f};
    health = 100;
    currentAmmo = 30;
    isShooting = false;
    isReloading = false;
}