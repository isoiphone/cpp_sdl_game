#include "SDLGame.h"
#include <assert.h>

//
///
SDLGamepad::SDLGamepad() {
    mCurState = mPrevState = mDeltaState = 0;
}

SDLGamepad::~SDLGamepad() {
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
                default:
                    break;
            }
        } else if (event.type == SDL_QUIT) {
            RequestQuit();
        }
    }
    
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
	gluPerspective(45.0, ((float)screenWidth/(float)screenHeight), 0.1, 100.0);
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    assert(glGetError() == GL_NO_ERROR);
    
    mRotation = 0.0f;
}

SDLGame::~SDLGame() {
}
    
void SDLGame::update(const Gamepad& gamepad) {
    mRotation -= 0.5f;
    
    if (gamepad.didPress(Gamepad::LEFT)) {
        mRotation -= 20.0f;
    } else if (gamepad.isDown(Gamepad::UP)) {
        mRotation += 5.0f;
    }
}

void SDLGame::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glTranslatef(0.0f, 0.0f, -6.0f);
    glRotatef(mRotation, 1.0f, 1.0f, 0.0f);
    
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
