#include "game.h"

int main()
{
    InitWindow(1280, 720, "ZombieAttack");
    SetTargetFPS(150);

    Game game;
    game.run(); 

    CloseWindow();
    return 0;
}