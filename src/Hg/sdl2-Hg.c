/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

#include "Hg.h"
#define HGL_LOG_IMPLEMENTATION
#include "HgL_Log.h"
#define HGL_ARENA_IMPLEMENTATION
#include "HgL_Arena.h"

bool isRunning = true;
float aspectRatio = (float)INIT_WIDTH / (float)INIT_HEIGHT; 

#include "platform/file.c"

#include "platform/gl/glad.h"
#include <SDL2/SDL.h>
#include "platform/gl/gl.c"

#include "objLoad.c"
#include "entity.c"
#include "camera.c"

#include <stdlib.h>
#include "game.h"
#include "../gameState.h"

//POSIX
#include <dlfcn.h>

HgInput input = {0};

HgGameState *gs;
HgArena *arena;

#define HOTLOAD_DELAY 0.5

void hgButtonDown(HgButtonInput *button){
  button->count++;
  button->isEndDown = true;
}

void hgButtonUp(HgButtonInput *button){
  button->count++;
  button->isEndDown = false;
}

void hgResetInput(void){
  input.left.isStartDown = input.left.isEndDown;
  input.right.isStartDown = input.right.isEndDown;
  input.up.isStartDown = input.up.isEndDown;
  input.down.isStartDown = input.down.isEndDown;
  input.accept.isStartDown = input.accept.isEndDown;
  input.cancel.isStartDown = input.cancel.isEndDown;
  input.alt.isStartDown = input.alt.isEndDown;
  input.menu.isStartDown = input.menu.isEndDown;
  input.start.isStartDown = input.start.isEndDown;
  input.select.isStartDown = input.select.isEndDown;
}

void hgProcessInput(void){
  SDL_Event e;
  
  //TODO impliment analog sticks
  input.isAnalog = false;
  hgResetInput();

  while(SDL_PollEvent(&e) != 0){
    if(e.type == SDL_QUIT){
      isRunning = false;
    }
    else if( e.type == SDL_KEYDOWN ){
      switch( e.key.keysym.sym ){
        case SDLK_LEFT: 
        case SDLK_a:
          hgButtonDown(&input.left);
          break;
        case SDLK_RIGHT: 
        case SDLK_d:
          hgButtonDown(&input.right);
          break;
        case SDLK_UP: 
        case SDLK_w:
          hgButtonDown(&input.up);
          break;
        case SDLK_DOWN: 
        case SDLK_s:
          hgButtonDown(&input.down);
          break;
        case SDLK_e:
          hgButtonDown(&input.accept);
          break;
        case SDLK_SPACE:
          hgButtonDown(&input.cancel);
          break;
        case SDLK_r:
          hgButtonDown(&input.alt);
          break;
        case SDLK_q:
          hgButtonDown(&input.menu);
          break;
        case SDLK_ESCAPE:
          hgButtonDown(&input.start);
          break;
        case SDLK_RETURN:
          hgButtonDown(&input.select);
          break;
      }
    }
    else if( e.type == SDL_KEYUP ){
      switch( e.key.keysym.sym ){
        case SDLK_LEFT: 
        case SDLK_a:
          hgButtonUp(&input.left);
          break;
        case SDLK_RIGHT: 
        case SDLK_d:
          hgButtonUp(&input.right);
          break;
        case SDLK_UP: 
        case SDLK_w:
          hgButtonUp(&input.up);
          break;
        case SDLK_DOWN: 
        case SDLK_s:
          hgButtonUp(&input.down);
          break;
        case SDLK_e:
          hgButtonUp(&input.accept);
          break;
        case SDLK_SPACE:
          hgButtonUp(&input.cancel);
          break;
        case SDLK_r:
          hgButtonUp(&input.alt);
          break;
        case SDLK_q:
          hgButtonUp(&input.menu);
          break;
        case SDLK_ESCAPE:
          hgButtonUp(&input.start);
          break;
        case SDLK_RETURN:
          hgButtonUp(&input.select);
          break;
      }
    }
  }
}

HgVersionInfo hgGetVersionInfoStub(void){
  return (HgVersionInfo){0, 0, 0, "NO_NAME_GAME"};
}

void hgStartGameStub(HgArena *arena, HgGameState *gs){
  (void)(arena);
  (void)(gs);
}

void hgGameLoopStub(HgArena *arena,
                    HgGameState *gs, 
                    HgInput *input, 
                    double delta){
  (void)(arena);
  (void)(gs);
  (void)(input);
  (void)(delta);
}

void hgEndGameStub(HgArena *arena, HgGameState *gs){
  (void)(arena);
  (void)(gs);
}

typedef struct HgGameCode {

  bool isValid;
  void* so;
  HgVersionInfo (*hgGetVersionInfo)(void);
  void (*hgStartGame)(HgArena *hgArena, HgGameState *gs);
  void (*hgGameLoop)(HgArena *arena,
                     HgGameState *gs,
                     HgInput *input, 
                     double delta);
  void (*hgEndGame)(HgArena *arena, HgGameState *gs);
  time_t modTime;

}HgGameCode;

//POSIX
void hgUnloadGameCode(HgGameCode *gameCode){
  gameCode->isValid = false;
  if(gameCode->so != NULL){
    dlclose(gameCode->so);
    gameCode->so = NULL;
  }
  gameCode->hgGetVersionInfo = &hgGetVersionInfoStub;
  gameCode->hgStartGame = &hgStartGameStub;
  gameCode->hgGameLoop = &hgGameLoopStub;
  gameCode->hgEndGame = &hgEndGameStub;
}

//POSIX
void hgLoadGameCode(HgGameCode *gameCode){
  gameCode->isValid = false;
  hgCopyFile(SO_FILENAME, SO_FILENAME_TEMP);
  gameCode->modTime = hgFileModTime(SO_FILENAME);
  gameCode->so = dlopen(SO_FILE_TEMP, RTLD_LOCAL | RTLD_NOW);
  if(gameCode->so == NULL){
    HG_ERROR("Failed to load %s: %s", SO_FILENAME_TEMP, dlerror());
    return;
  }
  
  gameCode->hgGetVersionInfo = 
    (HgVersionInfo (*)(void)) dlsym(gameCode->so, "hgGetVersionInfo"); 
  if(gameCode->hgGetVersionInfo == NULL){
    HG_ERROR("Failed to load func hgGetVersionInfo in %s", SO_FILENAME_TEMP);
    return;
  }

  gameCode->hgStartGame = 
      (void (*)(HgArena *arena, HgGameState *gs)) dlsym(gameCode->so, "hgStartGame"); 
  if(gameCode->hgStartGame == NULL){
    HG_ERROR("Failed to load func hgStartGame in %s", SO_FILENAME_TEMP);
    return;
  }

  gameCode->hgGameLoop = 
      (void (*)(HgArena *arena,
                HgGameState *gs,
                HgInput *input,
                double delta))
      dlsym(gameCode->so, "hgGameLoop");
  if(gameCode->hgGameLoop == NULL){
    HG_ERROR("Failed to load func hgGameLoop in %s", SO_FILENAME_TEMP);
    return;
  }
  
  gameCode->hgEndGame = 
      (void (*)(HgArena *arena, HgGameState *gs)) dlsym(gameCode->so, "hgEndGame");
  if(gameCode->hgEndGame == NULL){
    HG_ERROR("Failed to load func hgEndGame in %s", SO_FILENAME_TEMP);
    return;
  }
  
}

bool hgCheckGameHotLoad(HgGameCode *gameCode){
  time_t nowTime = hgFileModTime(SO_FILENAME);
  double diff = difftime(nowTime, gameCode->modTime);
  if (diff > 0.5){
    return true;
  }
  return false;
}

HgGameCode gameCode = {0};

int main(void)
{

  SDL_Window* window = NULL;
  SDL_GLContext context = NULL;

  if( SDL_Init(SDL_INIT_VIDEO) < 0){
    HG_FATAL("Failed to initialize SDL2! ERR: %s", SDL_GetError());
    return -1;
  }
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24);

  window = SDL_CreateWindow(WINDOW_NAME,
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            INIT_WIDTH,
                            INIT_HEIGHT,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
                            );
  if(window == NULL){
    HG_FATAL("Window could not be created! ERR: %s", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  context = SDL_GL_CreateContext(window); 
  if(context == NULL){
    HG_FATAL("GLES3 Context could not be created! ERR: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();

  }

  if (hgInitGL()) {
    HG_FATAL("Failed to init OpenGL!");
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  void *gameState = malloc(sizeof(HgGameState));
  if(gameState == NULL){
    HG_FATAL("Failed to malloc GameState memory!");
    return -1;
  }
 
  arena = hgCreateArena(ALLOC_MEM_SIZE, 16);

  gs = gameState; 
  
#ifdef HG_BUILD_DEBUG
  double delayReload = 0.0;
  hgLoadGameCode(&gameCode);
  HgVersionInfo vi = gameCode.hgGetVersionInfo();
  gameCode.hgStartGame(arena, gs);
  
  HG_LOG("MERCURY ENGINE ~ v%d.%d.%d", HG_MAJOR, HG_MINOR, HG_PATCH);
  HG_LOG("%s ~ v%d.%d.%d",vi.gameName, vi.major, vi.minor, vi.patch );
#else
  hgStartGame(arena, gs);
#endif //HG_BUILD_DEBUG

  uint64_t now = SDL_GetPerformanceCounter();
  uint64_t last = 0;
  
  double delta = 0.0;

  /* Loop until the user closes the window */
  while (isRunning)
  {
    last = now;
    now = SDL_GetPerformanceCounter();
    
    delta = ((now - last) / (double)SDL_GetPerformanceFrequency() );;

    /* Poll for and process events */
    hgProcessInput();



#ifdef HG_BUILD_DEBUG
    gameCode.hgGameLoop(arena, gs, &input, delta);
    if(hgCheckGameHotLoad(&gameCode)){
      delayReload += delta;
      if (delayReload > HOTLOAD_DELAY){
        delayReload = 0.0;
        hgUnloadGameCode(&gameCode);
        hgLoadGameCode(&gameCode);
      }
    }
#else
    hgGameLoop(arena, gs, &input, delta);
#endif //HG_BUILD_DEBUG

    /* Swap front and back buffers */
    SDL_GL_SwapWindow(window);

  }
#ifdef HG_BUILD_DEBUG
  gameCode.hgEndGame(arena, gs);
#else
  hgEndGame(arena, gs);
#endif //HG_BUILD_DEBUG

  if(meshShader.program){
    GL_CALL(glDeleteProgram(meshShader.program));
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  free(gameState);
  hgDestroyArena(arena);

#ifdef DEBUG
  dlclose(gameCode.so);
  hgDeleteFile(SO_FILENAME_TEMP);
#endif

  HG_LOG("Exiting.");

  return 0;
}
