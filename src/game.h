#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "bullet.h"
#include "zombie.h"
#include <vector>

struct Decal
{
    Vector2 position;
    float rotation;
    float scale;
    Texture2D texture;
    float lifeTime;
    float maxLife;
    bool onTop;
};

class Game
{
public:
    Game();
    ~Game();

    void run();

private:
    void update();
    void draw();
    bool gameOver;
    void resetGame();

    Player player; // game owns player
    Sound footstepSound;
    float footstepTimer;
    std::vector<Bullet> bullets;   // game owns bullets
    std::vector<Zombie *> zombies; // game owns zombies
    std::vector<Decal> decals;
    Texture2D backgroundTexture;

    // Shooting Settings
    float shootTimer;
    float fireRate;
    float gunOffset;
    float sideOffset;
    float noAmmoTimer;
    Sound shootSound;
    Sound reloadSound;
    float shakeTimer;
    int score;

    // cam
    Camera2D camera;

    // zomb
    float zombieSpawnTimer;
    float zombieSpawnRate;
    Texture2D muzzleFlashTexture; // bullet
    Texture2D hitTexture;         // blood
    Texture2D deadBodyTexture;    // blood spray
    // Texture2D shellTexture;
};

#endif