#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "raylib.h"
#include <vector>
#include "player.h"

class Zombie
{

public:
    Zombie(float x, float y);
    ~Zombie();

    void update(Player& player, std::vector<Zombie*>& zombies);
    void draw();
    Rectangle GetCollisionRect();
    bool takeDamage(int damage);
    bool active; // to check if its dead so we can clean it

private:
    Vector2 position;
    float speed;
    float rotation;
    float scale;
    int health;
    float attackTimer;

    // ANIM DATA
    std::vector<Texture2D> walkTextures;
    std::vector<Texture2D> attackTextures;

    int currentFrame;
    float frameTimer;
    float frameSpeed;

    bool isAttacking; // true = Biting, false = Chasing
};

#endif
