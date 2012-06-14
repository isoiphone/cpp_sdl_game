#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "game.h"

class SDLGamepad : public Gamepad {
public:
    SDLGamepad();
    virtual ~SDLGamepad();
    
    virtual bool isDown(EButton button) const;
    virtual bool didPress(EButton button) const;
    
    virtual void update();
//    virtual void save(std::ostream& fout);
//    virtual void load(std::istream& fin);
    
protected:
    Uint32 mCurState, mPrevState, mDeltaState;
    int mCurX, mCurY, mPrevX, mPrevY;
};


class SDLGame : public Game {
public:
    SDLGame(int screenWidth, int screenHeight);
    virtual ~SDLGame();
    
    virtual void update(const Gamepad& gamepad);
    virtual void render();
    
protected:
    float mRotation;
};

