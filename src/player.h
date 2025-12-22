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

private:
    Vector2 position;
    float speed;
    float rotation;

    // ANIM DATA
    std::vector<Texture2D> idleTextures;
    int currentFrame;
    float frameTimer;
    float frameSpeed;
};

#endif