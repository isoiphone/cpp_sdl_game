#include "SDLGame.h"
#include <assert.h>

//
///
SDLGamepad::SDLGamepad() {
    mCurState = mPrevState = mDeltaState = 0;
    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_ShowCursor(SDL_DISABLE);
}

SDLGamepad::~SDLGamepad() {
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    SDL_ShowCursor(SDL_ENABLE);
}

bool SDLGamepad::isDown(EButton button) const {
    return (mCurState & button);
}

bool SDLGamepad::didPress(EButton button) const {
    return (mCurState & button) && (mDeltaState & button);
}

void SDLGamepad::update() {
    SDL_Event event;

    mPrevState = mCurState;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
                case SDLK_a:
                case SDLK_LEFT:
                    mCurState |= LEFT;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    mCurState |= RIGHT;
                    break;
                case SDLK_w:
                case SDLK_UP:
                    mCurState |= UP;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    mCurState |= DOWN;
                    break;
                case SDLK_SPACE:
                    mCurState |= JUMP;
                    break;
                default:
                    break;
            }
        } else if (event.type == SDL_KEYUP) {
            switch(event.key.keysym.sym) {
                case SDLK_a:
                case SDLK_LEFT:
                    mCurState &= ~LEFT;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    mCurState &= ~RIGHT;
                    break;
                case SDLK_w:
                case SDLK_UP:
                    mCurState &= ~UP;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    mCurState &= ~DOWN;
                    break;
                case SDLK_SPACE:
                    mCurState &= ~JUMP;
                    break;
                #ifndef NDEBUG
                // I have no patience :(
                case SDLK_ESCAPE:
                    RequestQuit();
                    break;
                #endif
                default:
                    break;
            }
        } else if (event.type == SDL_QUIT) {
            RequestQuit();
        }
    }
    
    // get mouse button / position
    mPrevX = mCurX;
    mPrevY = mCurY;
    const Uint8 button = SDL_GetMouseState(&mCurX, &mCurY);
    
    // left button
    if (button&1)
        mCurState |= FIRE;
    else
        mCurState &= ~FIRE;
    
    // right button
    if (button&3)
        mCurState |= SPECIAL;
    else
        mCurState &= ~SPECIAL;
    
    //
    mDeltaState = mPrevState ^ mCurState;
}

//void SDLGamepad::save(std::ostream& fout) {
//    fout << mCurState;
//}
//
//void SDLGamepad::load(std::istream& fin) {
//    mPrevState = mCurState;
//    fin >> mCurState;
//    mDeltaState = mPrevState ^ mCurState;
//}


//
///
SDLGame::SDLGame(int screenWidth, int screenHeight) : Game(screenWidth, screenHeight) {
	glClearColor(0, 0, 0, 0);
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, ((float)screenWidth/(float)screenHeight), 0.1, 1000.0); // TODO: experiment with different FOVs
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    assert(glGetError() == GL_NO_ERROR);
    
    mX = 0.0f;
    mY = 1.8f;
    mZ = 6.0f;
    
    mPitch = mYaw = mRoll = 0;
}

SDLGame::~SDLGame() {
}
    
void SDLGame::update(const Gamepad& gamepad) {
    if (gamepad.isDown(Gamepad::LEFT)) {
        mX -= 0.05;
    } else if (gamepad.isDown(Gamepad::RIGHT)) {
        mX += 0.05;
    }
    
    if (gamepad.isDown(Gamepad::UP)) {
        mZ -= 0.05;
    } else if (gamepad.isDown(Gamepad::DOWN)) {
        mZ += 0.05;
    }
    
    if (gamepad.isDown(Gamepad::JUMP)) {
        mY += 0.05;
    } else {
        mY -= 0.05;
        
        if (mY < 1.8)
            mY = 1.8;
    }
    
    
    // sort of a SDL hack. reset mouse to middle of window so we never 'lose' it
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_WarpMouse(mScreenWidth/2, mScreenHeight/2);
    SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
}

void SDLGame::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    glTranslatef(-mX,-mY,-mZ);
    
    glBegin(GL_QUADS);
    
    // top
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    
    // bottom
    glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    
    // front
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    
    // back
    glColor4f(0.0f, 1.0f, 0.5f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    
    // left
    glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    
    // right
    glColor4f(0.15f, 0.25f, 0.75f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    
    glEnd();
    glLoadIdentity();
    
    SDL_GL_SwapBuffers();
}

//
///
Game* CreateGame(int screenWidth, int screenHeight) {
    return new SDLGame(screenWidth, screenHeight);
}

Gamepad* CreateGamepad() {
    return new SDLGamepad();
}
