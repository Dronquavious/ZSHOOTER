#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "bullet.h"
#include "zombie.h"
#include <vector>

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

    Player player;                 // game owns player
    std::vector<Bullet> bullets;   // game owns bullets
    std::vector<Zombie *> zombies; // game owns zombies
    Texture2D backgroundTexture;

    // Shooting Settings
    float shootTimer;
    float fireRate;
    float gunOffset;
    float sideOffset;

    // cam
    Camera2D camera;

    // zomb
    float zombieSpawnTimer;
    float zombieSpawnRate;
    
};

#endif