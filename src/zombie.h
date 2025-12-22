#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "raylib.h"
#include <vector>

class Zombie
{

public:
    Zombie(float x, float y);
    ~Zombie();

    void update(Vector2 playerPos);
    void draw();
    Rectangle GetCollisionRect();

private:
    Vector2 position;
    float speed;
    float rotation;
    float scale;

    // ANIM DATA
    std::vector<Texture2D> walkTextures;
    std::vector<Texture2D> attackTextures;

    int currentFrame;
    float frameTimer;
    float frameSpeed;

    bool isAttacking; // true = Biting, false = Chasing
};

#endif
