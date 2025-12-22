#include "game.h"
#include <cmath>
#include <algorithm> 

Game::Game() 
    : player(100.0f, 100.0f) 
{

    // init shooting vars
    shootTimer = 0.0f;
    fireRate = 0.2f;
    gunOffset = 35.0f;
    sideOffset = 15.0f;
}

Game::~Game()
{

}

void Game::run()
{
    // game loop
    while (!WindowShouldClose())
    {
        update();
        draw();
    }
}

void Game::update()
{
    float dt = GetFrameTime();
    shootTimer += dt;

    player.update();

    // --- SHOOTING LOGIC ---
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (shootTimer >= fireRate)
        {
            // reset timer
            shootTimer = 0.0f;
            
            player.shoot(); 

            // calculate Spawn
            Vector2 pPos = player.getPlayerPos();
            float rot = player.getPlayerRotation();
            float rads = rot * DEG2RAD;
            Vector2 forward = { cos(rads), sin(rads) };
            float rightRads = (rot + 90.0f) * DEG2RAD;
            Vector2 right = { cos(rightRads), sin(rightRads) };

            float spawnX = pPos.x + (forward.x * gunOffset) + (right.x * sideOffset);
            float spawnY = pPos.y + (forward.y * gunOffset) + (right.y * sideOffset);

            bullets.push_back(Bullet(spawnX, spawnY, rot));
        }
    }

    for (Bullet& b : bullets) {
        b.update();
    }

    // --- DELETE DEAD BULLETS ---
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), 
        [](const Bullet& b) { return !b.active; }), bullets.end());
    // ---------------------------------------
}

void Game::draw()
{
    BeginDrawing();
    ClearBackground(GRAY);

    player.draw();

    for (Bullet& b : bullets) {
        b.draw();
    }

    EndDrawing();
}