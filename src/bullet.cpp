#include "bullet.h"
#include <cmath>

Bullet::Bullet(float posX, float posY, float rotationArg)
{

    position.x = posX;
    position.y = posY;
    rotation = rotationArg;
    speed = 600.0f;
    active = true;
}

void Bullet::update()
{

    position.x += speed * cos(rotation * DEG2RAD) * GetFrameTime();
    position.y += speed * sin(rotation * DEG2RAD) * GetFrameTime();
    
    // if bullet too far offscreen delete it
    if (position.x > 1500 || position.x < -200 ||
        position.y > 1000 || position.y < -200)
    {
        active = false;
    }

}

void Bullet::draw()
{
    DrawCircle((int)position.x, (int)position.y, 5, RED);
}