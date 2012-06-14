#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "game.h"

static bool doLoop = true;

void RequestQuit()
{
    doLoop = false;
}

int main(int argc, char* args[])
{
    const int width=800, height=600, bpp=32;
    const char* caption = "FPS";
    
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        return 1;
    
    if (SDL_SetVideoMode(width, height, bpp, SDL_OPENGL) == 0)
        return 2;
    
    // vsync
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    
	SDL_WM_SetCaption(caption, 0);
    
    //
    Game* game = CreateGame(width, height);
    Gamepad* gamepad = CreateGamepad();
    
    Uint32 accum = 0;
    Uint32 lastTick = SDL_GetTicks();
    
    while (doLoop) {
        const Uint32 now = SDL_GetTicks();
        accum += now-lastTick;
        lastTick = now;
        
        while (accum > TICKS_PER_FRAME) {
            gamepad->update();
            game->update(*gamepad);
            accum -= TICKS_PER_FRAME;
        }
        game->render();
        
        // delay some amount of time to play nice with the OS
        // this value can be tweaked
        SDL_Delay(1);
    }
    
    delete game;
    delete gamepad;
    
    SDL_Quit();
    
    return 0;
}
