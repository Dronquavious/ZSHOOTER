#include "raylib.h"
#include "player.h"

int main()
{
    const int screenDimension[] = { 1280, 720 };

    InitWindow(screenDimension[0], screenDimension[1], "ZombieAttack");

    SetTargetFPS(150);

    // player stuff
    Player dron(100.0f,100.0f);

    while (!WindowShouldClose())
    {

        dron.update();

        float deltaTime = GetFrameTime();

        ClearBackground(RED);
        BeginDrawing();

        dron.draw();
        
        

        EndDrawing();
    }
    CloseWindow(); 

    return 0;
}