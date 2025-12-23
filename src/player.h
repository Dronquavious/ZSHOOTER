#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <vector>

class Player
{

public:
    Player(float x, float y);
    ~Player();

    void update(Vector2 mousePos);
    void draw();
    Vector2 getPlayerPos();
    float getPlayerRotation();
    void shoot();
    void reload(); // starts the reload process
    int getAmmo();
    bool isReloadingState();

private:
    Vector2 position;
    float speed;
    float rotation;
    float scale;

    int maxAmmo;
    int currentAmmo;

    // reload animation
    std::vector<Texture2D> reloadTextures;
    bool isReloading;

    // ANIM DATA
    std::vector<Texture2D> idleTextures;
    std::vector<Texture2D> shootTextures;
    int currentFrame;
    float frameTimer;
    float frameSpeed;
    bool isShooting;
};

#endif