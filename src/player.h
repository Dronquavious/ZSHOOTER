#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <vector>

class Player
{

public:
    Player(float x, float y);
    ~Player();

    void update();
    void draw();
    Vector2 getPlayerPos();
    float getPlayerRotation();
    void shoot();

private:
    Vector2 position;
    float speed;
    float rotation;
    float scale;


    // ANIM DATA
    std::vector<Texture2D> idleTextures;
    std::vector<Texture2D> shootTextures;
    int currentFrame;
    float frameTimer;
    float frameSpeed;
    bool isShooting;
};

#endif