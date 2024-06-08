#include "../Hg.c"

#include "file.c"

#include "gl/glad.h"
#include <SDL2/SDL.h>

#include "gl/gl.c"
#include "objLoad.c"

//POSIX
#include <dlfcn.h>

uint64_t now, last;

double delta = 0.0;

SDL_Window* window = NULL;
SDL_GLContext context = NULL;

/* Unload game shared object (DLL), and set pointers to stub functions*/
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

/* Load game shared object (DLL), and get function pointers for gameCode */
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

/* Check if game shared object (DLL) was recently updated */
bool hgCheckGameHotLoad(HgGameCode *gameCode){
  time_t nowTime = hgFileModTime(SO_FILENAME);
  double diff = difftime(nowTime, gameCode->modTime);
  if (diff > 0.5){
    return true;
  }
  return false;
}

/* Intitalize game engine */
int hgInitEngine(void){

  if( SDL_Init(SDL_INIT_VIDEO) < 0){
    HG_FATAL("Failed to initialize SDL2! ERR: %s", SDL_GetError());
    return -1;
  }
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24);

  window = SDL_CreateWindow(vi.gameName,
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

  now = SDL_GetPerformanceCounter();
  last = 0;
  
  return 0;

}

/* Calculate the delta time for new frame */
double hgCalculateDelta(void){
    last = now;
    now = SDL_GetPerformanceCounter();
    
    return ((now - last) / (double)SDL_GetPerformanceFrequency() );;

}

/* Get input updates from last frame and put them into HgInput struct */
void hgProcessInput(HgInput *input){
  SDL_Event e;
  
  //TODO impliment analog sticks
  input->isAnalog = false;
  hgResetInput(input);

  while(SDL_PollEvent(&e) != 0){
    if(e.type == SDL_QUIT){
      isRunning = false;
    }
    else if( e.type == SDL_KEYDOWN ){
      switch( e.key.keysym.sym ){
        case SDLK_LEFT: 
        case SDLK_a:
          hgButtonDown(&input->left);
          break;
        case SDLK_RIGHT: 
        case SDLK_d:
          hgButtonDown(&input->right);
          break;
        case SDLK_UP: 
        case SDLK_w:
          hgButtonDown(&input->up);
          break;
        case SDLK_DOWN: 
        case SDLK_s:
          hgButtonDown(&input->down);
          break;
        case SDLK_e:
          hgButtonDown(&input->accept);
          break;
        case SDLK_SPACE:
          hgButtonDown(&input->cancel);
          break;
        case SDLK_r:
          hgButtonDown(&input->alt);
          break;
        case SDLK_q:
          hgButtonDown(&input->menu);
          break;
        case SDLK_ESCAPE:
          hgButtonDown(&input->start);
          break;
        case SDLK_RETURN:
          hgButtonDown(&input->select);
          break;
      }
    }
    else if( e.type == SDL_KEYUP ){
      switch( e.key.keysym.sym ){
        case SDLK_LEFT: 
        case SDLK_a:
          hgButtonUp(&input->left);
          break;
        case SDLK_RIGHT: 
        case SDLK_d:
          hgButtonUp(&input->right);
          break;
        case SDLK_UP: 
        case SDLK_w:
          hgButtonUp(&input->up);
          break;
        case SDLK_DOWN: 
        case SDLK_s:
          hgButtonUp(&input->down);
          break;
        case SDLK_e:
          hgButtonUp(&input->accept);
          break;
        case SDLK_SPACE:
          hgButtonUp(&input->cancel);
          break;
        case SDLK_r:
          hgButtonUp(&input->alt);
          break;
        case SDLK_q:
          hgButtonUp(&input->menu);
          break;
        case SDLK_ESCAPE:
          hgButtonUp(&input->start);
          break;
        case SDLK_RETURN:
          hgButtonUp(&input->select);
          break;
      }
    }
  }
}

/* Called once per frame to update the screen */
void hgUpdateEngine(void){
    SDL_GL_SwapWindow(window);
}

/* Cleanup your memory */
void hgCleanupEngine(void){
  if(meshShader.program){
    GL_CALL(glDeleteProgram(meshShader.program));
  }
  SDL_DestroyWindow(window);
  SDL_Quit();
}
