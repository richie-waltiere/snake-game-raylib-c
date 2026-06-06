#include "raylib.h"
#include "stdlib.h"
#include "snake.h"
#include "time.h"

int main(void)
{
	srand(time(0));
    GameState *gs = GameInit();

    while (!WindowShouldClose())
    {
        gs = GameUpdate(gs);
        GameDraw(gs);
    }

    CloseWindow();
    free(gs);

    return 0;
}