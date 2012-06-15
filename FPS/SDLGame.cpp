#include "SDLGame.h"
#include <assert.h>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

//
///
SDLGamepad::SDLGamepad() {
    mCurState = mPrevState = mDeltaState = 0;
    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    
    // FIXME: TODO: use actual size of window here
    mX = 0; mY = 0;
}

SDLGamepad::~SDLGamepad() {
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    SDL_ShowCursor(SDL_ENABLE);
    SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);     
}

bool SDLGamepad::isDown(EButton button) const {
    return (mCurState & button);
}

bool SDLGamepad::didPress(EButton button) const {
    return (mCurState & button) && (mDeltaState & button);
}

void SDLGamepad::getStick(int* mx, int* my) const {
    *mx = mX;
    *my = mY;
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
    
    // FIXME: TODO: use actual size of window here
    
    // get mouse button / position
    int cx,cy;
    const Uint8 button = SDL_GetMouseState(&cx, &cy);

    mX = cx-400;
    mY = cy-300;

    printf("%d, %d\n", mX, mY);
    
    // SDL hack. reset mouse to middle of window so we never 'lose' it
    SDL_WarpMouse(400, 300); 
    
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
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    assert(glGetError() == GL_NO_ERROR);
    
    mYaw = M_PI;
    mPitch = 0.0f;
    mPosition = glm::vec3(0.0f, 0.0f, 10.0f);
}

SDLGame::~SDLGame() {
}
    
void SDLGame::update(const Gamepad& gamepad) {
    const float speed = 0.1f;
    
    if (gamepad.isDown(Gamepad::LEFT)) {
        mPosition -= mRight*speed;
    } else if (gamepad.isDown(Gamepad::RIGHT)) {
        mPosition += mRight*speed;
    }

    if (gamepad.isDown(Gamepad::UP)) {
        mPosition += mDirection*speed;
    } else if (gamepad.isDown(Gamepad::DOWN)) {
        mPosition -= mDirection*speed;
    }

    int dx,dy;
    gamepad.getStick(&dx, &dy);
    
//    printf("%d, %d\n", dx, dy);
    
    mYaw -= dx*0.005f;
    mPitch -= dy*0.005f;
    
    mDirection = glm::vec3(cos(mPitch) * sin(mYaw),
                           sin(mPitch),
                           cos(mPitch) * cos(mYaw));
    
    mRight = glm::vec3(sin(mYaw - M_PI_2),
                       0,
                       cos(mYaw - M_PI_2));
    
    mUp = glm::cross(mRight, mDirection);
}

void SDLGame::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    glm::mat4 projection = glm::perspective(45.0f, (float)mScreenWidth/(float)mScreenHeight, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(mPosition, mPosition+mDirection, mUp);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(view));
    
    
//    float avatarYaw = 0.0f;
//    
//    glLoadMatrixf(<#const GLfloat *m#>)
//    
//    glm::mat4 glm::rotate(glm::mat4(1.0f), avatarYaw, glm::vec3(0.0f, 0.0f, 1.0f));
//
////    glm::vec3 facing = glm::rotate(glm::mat4(1.0f), avatarYaw, glm::vec3(0.0f, 0.0f, 1.0f));
////    glm::vec3 lookAt = mPosition + facing;
////    glm::vec4 = Matrix.CreateLookAt(mPosition, cameraLookat, new Vector3(0.0f, 1.0f, 0.0f));
//    glm::lookAt(mPosition, mPosition, glm::vec3(0.0f, 0.0f, 1.0f))
//
//    glm::lookAt(glm::vec3(0.0f,0.0f, 0.0f),
//                glm::vec3(0.0f,0.0f,-1.0f),
//                glm::vec3(0.0f,1.0f, 0.0f));
    
    
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
