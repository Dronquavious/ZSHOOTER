#include "zombie.h"
#include <cmath>

Zombie::Zombie(float x, float y)
{

    position.x = x;
    position.y = y;
    speed = 100.0f;
    rotation = 0.0f;
    scale = 0.25f;
    isAttacking = false;
    health = 140;
    active = true;

    currentFrame = 0;
    frameTimer = 0.0f;
    frameSpeed = 0.1f;

    // zombie textures

    // walk
    for (int i = 0; i < 17; i++)
    {

        std::string fileName = "assets/zombie/walk/skeleton-move_" + std::to_string(i) +
                               ".png";

        Texture2D texture = LoadTexture(fileName.c_str());
        walkTextures.push_back(texture);
    }

    // attack
    for (int i = 0; i < 9; i++)
    {

        std::string fileName = "assets/zombie/attack/skeleton-attack_" + std::to_string(i) +
                               ".png";

        Texture2D texture = LoadTexture(fileName.c_str());
        attackTextures.push_back(texture);
    }
}

Zombie::~Zombie()
{

    // unloading texttures
    for (Texture2D texture : walkTextures)
    {
        UnloadTexture(texture);
    }

    for (Texture2D texture : attackTextures)
    {
        UnloadTexture(texture);
    }
}

void Zombie::update(Player& player, std::vector<Zombie*>& zombies)
{
    float dt = GetFrameTime();

    Vector2 playerPos = player.getPlayerPos();

    // vector to player
    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;

    // angle to face player
    rotation = atan2(dy, dx) * RAD2DEG;

    float distance = sqrt(dx * dx + dy * dy);

    // if we are > 30 pixels run, vice versa attack
    if (distance > 30.0f)
    {
        isAttacking = false;

        // normalize the vector so we move at constant speed
        float dirX = dx / distance;
        float dirY = dy / distance;

        position.x += dirX * speed * dt;
        position.y += dirY * speed * dt;
    }
    else
    {
        // we are close start attacking
        isAttacking = true;
        attackTimer += dt;
        if (attackTimer >= 1.0f) // bite every 1.0 seconds
        {
            player.takeDamage(10); // Ouch!
            attackTimer = 0.0f;    // reset timer
        }
    }

    // --- SEPARATION LOGIC (The Shove) ---
    for (Zombie* other : zombies)
    {
        // dont check against yourself, and dont check dead zombies
        if (other == this || !other->active) continue;

        float sdx = position.x - other->position.x;
        float sdy = position.y - other->position.y;
        float sDist = sqrt(sdx*sdx + sdy*sdy);

        // if they are inside our "Personal Space" (30 pixels)
        if (sDist < 30.0f)
        {
            // calculate a vector pointing AWAY from them
            // normalize it (so we push evenly)
            float pushX = sdx / sDist;
            float pushY = sdy / sDist;

            // push ourselves away (Move slightly faster than walk speed to resolve overlaps)
            position.x += pushX * 50.0f * dt;
            position.y += pushY * 50.0f * dt;
        }
    }

    // --- ANIM LOGIC ---

    frameTimer += dt;
    // choose which list to use based on state
    std::vector<Texture2D>& currentAnim = isAttacking ? attackTextures : walkTextures;

    if (frameTimer >= 0.1f) // 0.1f = 10 frames per second
    {
        frameTimer = 0.0f;
        currentFrame++;
        
        // loop the animation
        if (currentFrame >= currentAnim.size())
        {
            currentFrame = 0;
        }
    }
}

void Zombie::draw()
{

    std::vector<Texture2D>& currentAnim = isAttacking ? attackTextures : walkTextures;
    
    // make sure we dont crash if frame is too high
    if (currentFrame >= currentAnim.size()) currentFrame = 0;

    Texture2D currentTexture = currentAnim[currentFrame];

    Rectangle source = { 0, 0, (float)currentTexture.width, (float)currentTexture.height };
    Rectangle dest = { position.x, position.y, currentTexture.width * scale, currentTexture.height * scale };
    Vector2 origin = { dest.width / 2, dest.height / 2 };

    DrawTexturePro(currentTexture, source, dest, origin, rotation, WHITE);
    
    // Debug:  collision box to see where it is
    // DrawRectangleLinesEx(getCollisionRect(), 2, RED); 
}

Rectangle Zombie::GetCollisionRect()
{
    // a simple box centered on the zombie
    return { position.x - 20, position.y - 20, 40, 40 };
}

bool Zombie::takeDamage(int damage)
{
    health -= damage;
    if (health <= 0)
    {
        active = false; // mark for deletion
        return true;    // "I died"
    }
    return false;       // "still alive"
}

Vector2 Zombie::getPosition()
{
    return position;
}