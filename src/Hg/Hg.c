/*
 *  Author: Gwenivere Benzschawel
 *  Copyright 2024
 *  License: MIT
 *
 *  Purpose: This is the platform agnostic part of the Mercury game engine.
 *  To support platform to Mercury:
 *      1) impliment the functions listed below
 *      2) include file.c and the graphics api you want (only gl/gl.c for now)
 *      3) #include this file
 *      4) build your file linked with the game file
 *
 *      See platform/sdl2-Hg.c for an example
 */

#include "Hg.h"
#define HGL_LOG_IMPLEMENTATION
#include "HgL_Log.h"
#define HGL_ARENA_IMPLEMENTATION
#include "HgL_Arena.h"

typedef struct HgGameCode HgGameCode; /* Defined later in document */

/******************************
 * Impliment these functions! *
 ******************************/

/* Unload game shared object (DLL), and set pointers to stub functions*/
void hgUnloadGameCode(HgGameCode *gameCode);

/* Load game shared object (DLL), and get function pointers for gameCode */
void hgLoadGameCode(HgGameCode *gameCode);

/* Check if game shared object (DLL) was recently updated */
bool hgCheckGameHotLoad(HgGameCode *gameCode);

/* Intitalize game engine */
int hgInitEngine(void);

/* Calculate the delta time for new frame */
double hgCalculateDelta(void);

/* Get input updates from last frame and put them into HgInput struct */
void hgProcessInput(HgInput *input);

/* Called once per frame to update the screen */
void hgUpdateEngine(void);

/* Cleanup your memory */
void hgCleanupEngine(void);

/******************************
 * Platform Independent Code: *
 ******************************/

/* Mercury Engine Version (01.02) */
#define HG_MAJOR 0
#define HG_MINOR 2
#define HG_PATCH 0

/* Shared Object (DLL) Filename Defs (01.03) */
#ifdef HG_BUILD_DEBUG
#define SO_FILE "./libGameDbg.so"
#define SO_FILE_TEMP "./libGameDbg_temp.so"
#define SO_FILENAME "libGameDbg.so"
#define SO_FILENAME_TEMP "libGameDbg_temp.so"
#else
#define SO_FILE "./libGame.so"
#define SO_FILE_TEMP "./libGame_temp.so"
#define SO_FILENAME "libGame.so"
#define SO_FILENAME_TEMP "libGame_temp.so"
#endif /* HG_BUILD_DEBUG */

#include "entity.c"
#include "camera.c"

#include "game.h"
#include "../gameState.h"

/* Shared Object (DLL) function pointers */
struct HgGameCode {
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
};

HgGameCode gameCode = {0};
HgInput input = {0};

bool isRunning = true;

HgGameState *gs;
HgArena *arena;

HgVersionInfo vi;

#define HOTLOAD_DELAY 0.5

/* Input */
void hgButtonDown(HgButtonInput *button){
  button->count++;
  button->isEndDown = true;
}

void hgButtonUp(HgButtonInput *button){
  button->count++;
  button->isEndDown = false;
}

void hgResetInput(HgInput *input){
  input->left.isStartDown = input->left.isEndDown;
  input->right.isStartDown = input->right.isEndDown;
  input->up.isStartDown = input->up.isEndDown;
  input->down.isStartDown = input->down.isEndDown;
  input->accept.isStartDown = input->accept.isEndDown;
  input->cancel.isStartDown = input->cancel.isEndDown;
  input->alt.isStartDown = input->alt.isEndDown;
  input->menu.isStartDown = input->menu.isEndDown;
  input->start.isStartDown = input->start.isEndDown;
  input->select.isStartDown = input->select.isEndDown;

  input->left.count = 0;
  input->right.count = 0;
  input->up.count = 0;
  input->down.count = 0;
  input->accept.count = 0;
  input->cancel.count = 0;
  input->alt.count = 0;
  input->menu.count = 0;
  input->start.count = 0;
  input->select.count = 0;
}

/* Shared Object (DLL) stub functions */

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

int main(void){

  #ifdef HG_BUILD_DEBUG
  double delayReload = 0.0;
  hgLoadGameCode(&gameCode);
  vi = gameCode.hgGetVersionInfo();
  
  HG_LOG("MERCURY ENGINE ~ v%d.%d.%d", HG_MAJOR, HG_MINOR, HG_PATCH);
  HG_LOG("%s ~ v%d.%d.%d",vi.gameName, vi.major, vi.minor, vi.patch );

#else
  vi = hgGetVersionInfo();
#endif

  int err = hgInitEngine();
  if(err){return err;}
  
  void *gameState = malloc(sizeof(HgGameState));
  if(gameState == NULL){
    HG_FATAL("Failed to malloc GameState memory!");
    return -1;
  }
 
  arena = hgCreateArena(ALLOC_MEM_SIZE, 16);

  gs = gameState; 

#ifdef HG_BUILD_DEBUG
  gameCode.hgStartGame(arena, gs);
#else
  hgStartGame(arena, gs);
#endif //HG_BUILD_DEBUG
  double delta;
  while(isRunning){
    delta = hgCalculateDelta();
    hgProcessInput(&input);

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

    hgUpdateEngine();
  }

#ifdef HG_BUILD_DEBUG
  gameCode.hgEndGame(arena, gs);
#else
  hgEndGame(arena, gs);
#endif //HG_BUILD_DEBUG
  
  hgCleanupEngine();
  HG_LOG("Exiting");

}
