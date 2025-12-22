#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"

class Bullet
{

public:
    Bullet(float posX, float posY, float rotationArg);

    // may get its own texture
    //~Bullet();

    void update();
    void draw();

    bool active;

private:
    Vector2 position;
    float speed;
    float rotation;
    
};

#endif