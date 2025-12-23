#include "game.h"
#include <cmath>
#include <algorithm>

Game::Game()
    : player(100.0f, 100.0f)
{

    InitAudioDevice();
    shootSound = LoadSound("assets/sounds/rifle-single.wav");
    reloadSound = LoadSound("assets/sounds/rifle-reload2.wav");

    footstepSound = LoadSound("assets/sounds/step-dirt.ogg");

    // lower volume
    SetSoundVolume(footstepSound, 0.4f);

    footstepTimer = 0.0f;

    backgroundTexture = LoadTexture("assets/map/backround.png");

    muzzleFlashTexture = LoadTexture("assets/effects/bullet2.png"); // the flash
    hitTexture = LoadTexture("assets/effects/blood.png");           // the hit splat
    deadBodyTexture = LoadTexture("assets/effects/bloodspray.png");
    gameOver = false;

    // init shooting vars
    shootTimer = 0.0f;
    fireRate = 0.09f;
    gunOffset = 35.0f;
    sideOffset = 15.0f;
    noAmmoTimer = 0.0f;
    score = 0;
    shakeTimer = 0;

    // camera
    camera.offset = {1280.0f / 2.0f, 720.0f / 2.0f};
    camera.target = player.getPlayerPos();
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

    // zombie
    zombieSpawnTimer = 0.0f;
    zombieSpawnRate = 2.0f; // Spawn every 2 seconds

    // Zombie TEST
    int hordeSize = 50; // change this to 100 or 200 if you are brave

    for (int i = 0; i < hordeSize; i++)
    {
        // pick a random direction
        float angle = GetRandomValue(0, 360) * DEG2RAD;

        // pick a random distance (Far enough to be safe, close enough to be scary)
        float distance = GetRandomValue(600, 1200);

        // calculate position relative to player start (100, 100)
        float spawnX = 100.0f + cos(angle) * distance;
        float spawnY = 100.0f + sin(angle) * distance;

        zombies.push_back(new Zombie(spawnX, spawnY));
    }
}

Game::~Game()
{
    for (Zombie *z : zombies)
    {
        delete z;
    }
    zombies.clear();

    UnloadTexture(backgroundTexture);
    UnloadTexture(muzzleFlashTexture);
    UnloadTexture(hitTexture);
    UnloadTexture(deadBodyTexture);

    CloseAudioDevice();
    UnloadSound(shootSound);
    UnloadSound(reloadSound);
    UnloadSound(footstepSound);
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

    // --- UPDATE DECALS (Fade them out) ---
    for (int i = 0; i < decals.size(); i++)
    {
        decals[i].lifeTime -= dt;

        if (decals[i].lifeTime <= 0)
        {
            decals.erase(decals.begin() + i);
            i--;
        }
    }

    // --- FOOTSTEP LOGIC ---
    bool isMoving = IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D);

    if (isMoving && !player.isReloadingState()) // dont play steps if we are frozen reloading
    {
        footstepTimer -= dt;

        if (footstepTimer <= 0.0f)
        {
            // PLAY SOUND
            // randomize pitch slightly so it doesn't sound like a robot
            SetSoundPitch(footstepSound, (float)GetRandomValue(90, 110) / 100.0f);
            PlaySound(footstepSound);

            // RESET TIMER
            // 0.30f = Run speed (approx 3 steps per second)
            footstepTimer = 0.30f;
        }
    }
    else
    {
        // reset so the first step plays instantly when you start moving again
        footstepTimer = 0.0f;
    }

    shootTimer += dt;

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
    player.update(mouseWorldPos);

    zombieSpawnTimer += dt;

    if (zombieSpawnTimer >= zombieSpawnRate)
    {
        zombieSpawnTimer = 0.0f; // reset timer

        // a random angle (0 to 360 degrees)
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

        // decrease spawn rate (make it faster) but dont go below 0.2 seconds
        if (zombieSpawnRate > 0.2f)
        {
            zombieSpawnRate -= 0.05f;
        }
    }

    // --- SHOOTING LOGIC ---
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (shootTimer >= fireRate && !player.isReloadingState() && player.getAmmo() > 0)
        {
            // reset timer
            shootTimer = 0.0f;

            player.shoot();
            PlaySound(shootSound);
            // when you shoot or take damage:
            shakeTimer = 0.2f; // shake for 0.2 seconds

            // calculate Spawn
            Vector2 pPos = player.getPlayerPos();
            float rot = player.getPlayerRotation();
            float rads = rot * DEG2RAD;
            Vector2 forward = {cos(rads), sin(rads)};
            float rightRads = (rot + 90.0f) * DEG2RAD;
            Vector2 right = {cos(rightRads), sin(rightRads)};

            float spawnX = pPos.x + (forward.x * gunOffset) + (right.x * sideOffset);
            float spawnY = pPos.y + (forward.y * gunOffset) + (right.y * sideOffset);

            // --- SPAWN MUZZLE FLASH ---
            Decal flash;
            float extraPush = 20.0f;
            flash.position.x = spawnX + (forward.x * extraPush);
            flash.position.y = spawnY + (forward.y * extraPush);
            flash.onTop = true;
            flash.rotation = rot;
            flash.scale = 0.15f;
            flash.texture = muzzleFlashTexture;
            flash.lifeTime = 0.05f; // fast flash
            flash.maxLife = 0.05f;
            decals.push_back(flash);

            bullets.push_back(Bullet(spawnX, spawnY, rot));
        }
    }

    for (Bullet &b : bullets)
    {
        b.update();
    }

    // --- NO AMMO LOGIC ---
    // decrease the timer if it's running
    if (noAmmoTimer > 0.0f)
    {
        noAmmoTimer -= dt;
    }

    if (IsKeyPressed(KEY_R))
    {
        // only play sound if we actually start reloading
        if (!player.isReloadingState() && player.getAmmo() < 45)
        {
            player.reload();
            PlaySound(reloadSound);
        }
    }

    // check if player tries to shoot with empty mag
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (player.getAmmo() <= 0 && !player.isReloadingState())
        {
            noAmmoTimer = 0.5f; // Show text for 0.5 seconds
        }
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
                // ---  SPAWN BLOOD SPLAT ---
                Decal blood;
                blood.onTop = false;
                // need to access zombie position directly.
                // since Zombie::position is private, use z->GetCollisionRect() center for now
                Rectangle zRect = z->GetCollisionRect();
                blood.position = {zRect.x + zRect.width / 2, zRect.y + zRect.height / 2};

                blood.rotation = (float)GetRandomValue(0, 360);
                blood.scale = 0.5f;
                blood.texture = hitTexture;
                blood.lifeTime = 2.0f;
                blood.maxLife = 2.0f;
                decals.push_back(blood);

                break; // bullet can only hit one zombie!
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

            score += 100;
            // --- SPAWN DEAD BODY ---
            Decal body;
            body.onTop = false;

            // get Zombie Center (The safest way)
            Rectangle zRect = (*it)->GetCollisionRect();
            Vector2 zPos = {zRect.x + zRect.width / 2, zRect.y + zRect.height / 2};
            Vector2 pPos = player.getPlayerPos();

            // calculate Direction (From Player -> To Zombie)
            float dx = zPos.x - pPos.x;
            float dy = zPos.y - pPos.y;
            float angleRad = atan2(dy, dx);
            float angleDeg = angleRad * RAD2DEG;

            // offset: Start at Zombie and move 60 pixels "Behind" them
            // (Continuing the line from the player)
            body.position.x = zPos.x + (cos(angleRad) * 60.0f);
            body.position.y = zPos.y + (sin(angleRad) * 60.0f);

            // rotation and Size
            body.rotation = angleDeg;
            body.scale = 0.15f; // splatter tweaked size

            body.texture = deadBodyTexture;
            body.lifeTime = 60.0f;
            body.maxLife = 60.0f;
            decals.push_back(body);
            // --------------------------

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

    if (shakeTimer > 0)
    {
        float intensity = 3.0f; // how violent the shake is
        camera.offset.x = (1280 / 2) + GetRandomValue(-intensity, intensity);
        camera.offset.y = (720 / 2) + GetRandomValue(-intensity, intensity);
        shakeTimer -= GetFrameTime();
    }
    else
    {
        // reset to center
        camera.offset = {1280.0f / 2.0f, 720.0f / 2.0f};
    }

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

    // loop ground stuff
    for (Decal &d : decals)
    {
        if (d.onTop)
            continue; // skip flashes for now

        float alpha = d.lifeTime / d.maxLife;
        if (alpha > 1.0f)
            alpha = 1.0f;
        Color tint = Fade(WHITE, alpha);

        Rectangle source = {0, 0, (float)d.texture.width, (float)d.texture.height};
        Rectangle dest = {d.position.x, d.position.y, d.texture.width * d.scale, d.texture.height * d.scale};
        Vector2 origin = {dest.width / 2, dest.height / 2};

        DrawTexturePro(d.texture, source, dest, origin, d.rotation, tint);
    }

    player.draw();

    for (Zombie *z : zombies)
    {
        z->draw();
    }

    // loop ontop stuff
    for (Decal &d : decals)
    {
        if (!d.onTop)
            continue; // Skip ground stuff!

        float alpha = d.lifeTime / d.maxLife;
        if (alpha > 1.0f)
            alpha = 1.0f;
        Color tint = Fade(WHITE, alpha);

        Rectangle source = {0, 0, (float)d.texture.width, (float)d.texture.height};
        Rectangle dest = {d.position.x, d.position.y, d.texture.width * d.scale, d.texture.height * d.scale};
        Vector2 origin = {dest.width / 2, dest.height / 2};

        DrawTexturePro(d.texture, source, dest, origin, d.rotation, tint);
    }

    for (Bullet &b : bullets)
    {
        b.draw();
    }

    EndMode2D();

    // UI HERE
    if (gameOver)
    {
        const char *titleText = "GAME OVER";
        int titleFontSize = 80;
        const char *subText = "Press ENTER to Restart";
        int subFontSize = 30;
        int titleWidth = MeasureText(titleText, titleFontSize);
        int subWidth = MeasureText(subText, subFontSize);
        DrawText(titleText, (1280 / 2) - (titleWidth / 2), (720 / 2) - 60, titleFontSize, RED);
        DrawText(subText, (1280 / 2) - (subWidth / 2), (720 / 2) + 40, subFontSize, DARKGRAY);
    }
    else
    {
        // draw Health (Top Left)
        DrawText(TextFormat("Health: %i", player.getHealth()), 20, 20, 20, RED);

        // draw Ammo (Below Health)
        std::string ammoText = "Ammo: " + std::to_string(player.getAmmo()) + " / 45";
        DrawText(ammoText.c_str(), 20, 50, 20, YELLOW);

        // draw score
        // top Right Corner, subtract the text width from 1260 to keep it right allin
        const char *scoreText = TextFormat("Score: %i", score);
        int scoreWidth = MeasureText(scoreText, 20);
        DrawText(scoreText, 1280 - scoreWidth - 20, 20, 20, WHITE);

        // RELOAD TEXT (Big, Center Bottom)
        if (player.isReloadingState())
        {
            const char *reloadText = "RELOADING...";
            int fontSize = 40;
            int textWidth = MeasureText(reloadText, fontSize);

            // draw at horizontal center, near the bottom (Y = 600)
            DrawText(reloadText, (1280 / 2) - (textWidth / 2), 600, fontSize, RED);
        }

        // NO AMMO POPUP (Center Screen)
        if (noAmmoTimer > 0.0f)
        {
            const char *emptyText = "NO AMMO!";
            int fontSize = 50;
            int textWidth = MeasureText(emptyText, fontSize);

            // draw right in the middle of the screen
            DrawText(emptyText, (1280 / 2) - (textWidth / 2), (720 / 2) - 100, fontSize, RED);
        }
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
    decals.clear();

    // Spawn a new test zombie
    zombies.push_back(new Zombie(400, 400));

    gameOver = false;

    zombieSpawnTimer = 0.0f; // reset spawner
    zombieSpawnRate = 2.0f;  // reset difficulty
}