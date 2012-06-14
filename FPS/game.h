#pragma once
#include <iostream>

#define TICKS_PER_FRAME (1000/60)

class Gamepad {
public:
    enum EButton {  NONE=0,
                    UP=(1<<0),
                    DOWN=(1<<1),
                    LEFT=(1<<2),
                    RIGHT=(1<<3),
                    JUMP=(1<<4),
                    FIRE=(1<<5) };
    
    virtual bool isDown(EButton button) const =0;
    virtual bool didPress(EButton button) const =0;
    
    virtual void update()=0;
//    virtual void save(std::ostream& fout)=0;
//    virtual void load(std::istream& fin)=0;
};


class Game {
public:
    Game(int screenWidth, int screenHeight) : mScreenWidth(screenWidth), mScreenHeight(screenHeight) {}
    virtual ~Game() {}
    
    virtual void update(const Gamepad& gamepad)=0;
    virtual void render()=0;
    
protected:
    int mScreenWidth, mScreenHeight;
};

extern void RequestQuit();
extern Game* CreateGame(int screenWidth, int screenHeight);
extern Gamepad* CreateGamepad();