#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "bullet.h"
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

    Player player; // game owns player
    std::vector<Bullet> bullets; // game owns bullets
    
    // Shooting Settings
    float shootTimer;
    float fireRate;
    float gunOffset;
    float sideOffset;
    Camera2D camera;
};

#endif