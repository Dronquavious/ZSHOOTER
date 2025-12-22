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

    // camera
    camera.offset = {1280.0f / 2.0f, 720.0f / 2.0f};
    camera.target = player.getPlayerPos();
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
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

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
    player.update(mouseWorldPos);

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
            Vector2 forward = {cos(rads), sin(rads)};
            float rightRads = (rot + 90.0f) * DEG2RAD;
            Vector2 right = {cos(rightRads), sin(rightRads)};

            float spawnX = pPos.x + (forward.x * gunOffset) + (right.x * sideOffset);
            float spawnY = pPos.y + (forward.y * gunOffset) + (right.y * sideOffset);

            bullets.push_back(Bullet(spawnX, spawnY, rot));
        }
    }

    for (Bullet &b : bullets)
    {
        b.update();
    }

    // --- DELETE DEAD BULLETS ---
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet &b)
                                 { return !b.active; }),
                  bullets.end());
    // ---------------------------------------

    // CAMERA LOGIC
    camera.target = player.getPlayerPos();
}

void Game::draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);

    // World Bounds (Just a gray box so we can see if we are moving)
    DrawRectangle(-5000, -5000, 10000, 10000, DARKGRAY);
    DrawRectangle(0, 0, 1280, 720, GRAY); // Draw the "original" screen box for reference

    player.draw();

    for (Bullet &b : bullets)
    {
        b.draw();
    }

    EndMode2D();

    // UI HERE (Score, Ammo) FOR TESTING
    DrawText("Health: 100", 20, 20, 20, RED);

    EndDrawing();
}