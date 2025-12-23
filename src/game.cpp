#include "game.h"
#include <cmath>
#include <algorithm>

Game::Game()
    : player(100.0f, 100.0f)
{

    backgroundTexture = LoadTexture("assets/map/backround.png");

    // init shooting vars
    shootTimer = 0.0f;
    fireRate = 0.2f;
    gunOffset = 35.0f;
    sideOffset = 15.0f;

    // camera
    camera.offset = {1280.0f / 2.0f, 720.0f / 2.0f};
    camera.target = player.getPlayerPos();
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

    // zombie
    zombieSpawnTimer = 0.0f;
    zombieSpawnRate = 2.0f; // Spawn every 2 seconds

    // Zombie TEST
    zombies.push_back(new Zombie(400.0f, 400.0f));
}

Game::~Game()
{
    for (Zombie *z : zombies)
    {
        delete z;
    }
    zombies.clear();

    UnloadTexture(backgroundTexture);
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

    if (gameOver && IsKeyPressed(KEY_ENTER))
    {
        resetGame();
    }

    if (gameOver)
        return;

    if (player.getHealth() <= 0)
    {
        gameOver = true;
    }

    float dt = GetFrameTime();
    shootTimer += dt;

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
    player.update(mouseWorldPos);

    zombieSpawnTimer += dt;

    if (zombieSpawnTimer >= zombieSpawnRate)
    {
        zombieSpawnTimer = 0.0f; // reset timer

        // arandom angle (0 to 360 degrees)
        int angleDeg = GetRandomValue(0, 360);
        float angleRad = angleDeg * DEG2RAD;

        // choose distance (1000 pixels away so they spawn off-screen)
        float dist = 1000.0f;

        // calculate Spawn Position (PlayerPos + Offset)
        Vector2 pPos = player.getPlayerPos();
        float spawnX = pPos.x + cos(angleRad) * dist;
        float spawnY = pPos.y + sin(angleRad) * dist;

        // create the Zombie
        zombies.push_back(new Zombie(spawnX, spawnY));

        // Optional: Make the game harder over time
        // if (zombieSpawnRate > 0.5f) zombieSpawnRate -= 0.01f;
    }

    // --- SHOOTING LOGIC ---
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (shootTimer >= fireRate && !player.isReloadingState() && player.getAmmo() > 0)
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

    // --- COLLISION LOGIC (Bullet vs Zombie) ---
    for (Bullet &b : bullets)
    {
        if (!b.active)
            continue; // skip dead bullets

        for (Zombie *z : zombies)
        {
            if (!z->active)
                continue; // skip dead zombies

            // check Collision
            if (CheckCollisionCircleRec(
                    {b.position.x, b.position.y}, // Bullet Pos
                    5,                            // Bullet Radius
                    z->GetCollisionRect()         // Zombie Box
                    ))
            {
                // HIT!
                z->takeDamage(35); // zombie takes damage
                b.active = false;  // bullet is destroyed
                break;             // bullet can only hit one zombie!
            }
        }
    }

    for (Zombie *z : zombies)
    {
        z->update(player, zombies);
    }

    // --- DELETE DEAD BULLETS ---
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet &b)
                                 { return !b.active; }),
                  bullets.end());
    // ---------------------------------------

    // CAMERA LOGIC
    camera.target = player.getPlayerPos();

    // --- DELETE DEAD ZOMBIES ---
    auto it = zombies.begin();
    while (it != zombies.end())
    {
        if (!(*it)->active) // if zombie is dead
        {
            delete *it;             // delete the memory
            it = zombies.erase(it); // remove from list & update iterator
        }
        else
        {
            ++it; // move to next zombie
        }
    }
}

void Game::draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);

    // World Bounds (Just a gray box so we can see if we are moving)
    // DrawRectangle(-5000, -5000, 10000, 10000, DARKGRAY);
    // DrawRectangle(0, 0, 1280, 720, GRAY); // Draw the "original" screen box for reference

    // map
    int mapSize = 10000;
    int startX = -5000;
    int startY = -5000;
    int tileW = backgroundTexture.width;
    int tileH = backgroundTexture.height;

    // safety check to prevent infinite loop if texture didn't load (width=0)
    if (tileW > 0 && tileH > 0)
    {
        // loop through X axis
        for (int x = startX; x < startX + mapSize; x += tileW)
        {
            // loop through Y axis
            for (int y = startY; y < startY + mapSize; y += tileH)
            {
                DrawTexture(backgroundTexture, x, y, WHITE);
            }
        }
    }
    else
    {
        // fallback if no texture found
        DrawRectangle(startX, startY, mapSize, mapSize, DARKGRAY);
    }

    player.draw();

    for (Zombie *z : zombies)
    {
        z->draw();
    }

    for (Bullet &b : bullets)
    {
        b.draw();
    }

    EndMode2D();

    // UI HERE (Score, Ammo) FOR TESTING
    if (gameOver)
    {
        // the text and size
        const char *titleText = "GAME OVER";
        int titleFontSize = 80;

        const char *subText = "Press ENTER to Restart";
        int subFontSize = 30;

        // measure widths
        int titleWidth = MeasureText(titleText, titleFontSize);
        int subWidth = MeasureText(subText, subFontSize);

        // draw centered (ScreenCenter - HalfTextWidth)
        DrawText(titleText, (1280 / 2) - (titleWidth / 2), (720 / 2) - 60, titleFontSize, RED);
        DrawText(subText, (1280 / 2) - (subWidth / 2), (720 / 2) + 40, subFontSize, DARKGRAY);
    }
    else
    {
        // Normal UI
        DrawText(TextFormat("Health: %i", player.getHealth()), 20, 20, 20, RED);

        // create the string "Ammo: X / 30"
        std::string ammoText = "Ammo: " + std::to_string(player.getAmmo()) + " / 30";

        // draw it in yellow just below the health
        DrawText(ammoText.c_str(), 20, 50, 20, YELLOW);
    }

    EndDrawing();
}

void Game::resetGame()
{
    player.reset();

    // Delete all existing zombies
    for (Zombie *z : zombies)
        delete z;
    zombies.clear();

    // Delete all bullets
    bullets.clear();

    // Spawn a new test zombie
    zombies.push_back(new Zombie(400, 400));

    gameOver = false;

    zombieSpawnTimer = 0.0f; // reset spawner
    zombieSpawnRate = 2.0f;  // reset difficulty
}