/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 *
 *  Purpose: Hg.h is the documentation for using the mercury engine.
 *  It's also the header file for connecting the game translation unit, and
 *  the engine translation unit.
 */

#ifndef HG_H
#define HG_H

/**************************************************************************
 *                   Basic helper macros and includes                     *
 **************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

//Version
#define HG_MAJOR 0
#define HG_MINOR 2
#define HG_PATCH 0

//Shared Object
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
#endif //HG_BUILD_DEBUG

//Window
#define INIT_WIDTH 640
#define INIT_HEIGHT 480
#define INIT_ASPECT_RATIO (float)INIT_WIDTH / (float)INIT_HEIGHT

//Log
#define LOG_LENGTH 1028

//File
#define PATH_LENGTH 512

//camera
#define CAMERA_FOV (90 * M_PI / 180.0)
#define CAMERA_NEAR 0.1
#define CAMERA_FAR 100

//memory allocation
#define ALLOC_MEM_SIZE GIGABYTES(1)
#include <cglm/cglm.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif //M_PI

#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

#define KILOBYTES(x) ((x) * 1024) 
#define MEGABYTES(x) (KILOBYTES(x) * 1024) 
#define GIGABYTES(x) ( (size_t) MEGABYTES(x) * 1024) 

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

/**************************************************************************
 *                                  Engine                                *
 **************************************************************************/

/*******
 * Log *
 *******/

#include "HgL_Log.h"

/*********
 * Input *
 *********/
typedef struct HgButtonInput{
  bool isStartDown;
  bool isEndDown;
  uint32_t count;
}HgButtonInput;

typedef struct HgStickInput{
  int startX;
  int startY;
  int endX;
  int endY;
}HgStickInput;

typedef struct HgInput{
  bool isAnalog;
  HgButtonInput left;
  HgButtonInput right;
  HgButtonInput up;
  HgButtonInput down;
  HgButtonInput accept;
  HgButtonInput cancel;
  HgButtonInput alt;
  HgButtonInput menu;
  HgButtonInput start;
  HgButtonInput select;
  HgStickInput leftStick;
  HgStickInput rightStick;
}HgInput;

/**********
 * Memory *
 **********/

#include "HgL_Arena.h"

/********
 * Mesh *
 ********/
#define MESH_SHADER_FILE "mesh"

typedef struct HgMesh HgMesh;

void hgCleanupMesh(HgArena *hgArena, HgMesh *hgMesh);

/**********
 * Camera *
 **********/
typedef struct HgCamera {
  mat4 proj;
  mat4 view;
}HgCamera;

/**********
 * Entity *
 **********/
typedef struct HgEntity{
  mat4 trans;
  HgMesh *mesh;
  vec2 padding;
}HgEntity;

void hgInitEntity(HgEntity *hgEntity, HgMesh *hgMesh);

/***********
 * ObjLoad *
 ***********/
HgMesh* hgLoadObjMesh(HgArena *hgArena,
                      const char* file,
                      const char* object);

#define hgLoadMesh(a, b) hgLoadObjMesh(a, b, b)


/**************************************************************************
 *                                  Platform                              *
 **************************************************************************/

/**********
 * Shader *
 **********/
typedef struct HgShader HgShader;

HgShader hgCreateShader(HgArena *hgArena, const char *file);

void hgBindShader(HgShader *s);

void hgBindMeshShader(void);

void hgUnbindShader(void);

void hgCleanupShader(HgArena *hgArena, HgShader *s);
#include "uniform.h"

/****************
 * VertexBuffer *
 ****************/
typedef struct HgVertexBuffer HgVertexBuffer;

typedef struct HgVertex {
  vec3 position;  
  vec3 normal;
  vec2 texture;
}HgVertex;

void hgCreateMeshVertexBuffer(HgMesh *hgMesh,
                              HgVertex* data,
                              uint32_t vertCount,
                              uint32_t vertexSize,
                              uint16_t* inds,
                              uint32_t indCount);

void hgBindVertexBuffer(HgVertexBuffer *vb, HgShader *s);

void hgUnbindVertexBuffer(void);

void hgCleanupVertexBuffer(HgVertexBuffer *vb);

/***********
 * Texture *
 ***********/
typedef struct HgTexture HgTexture;

void hgBindTexture(HgTexture *t, uint32_t slot);

void hgUnbindTexture(void);

void hgLoadMeshTexture(HgMesh *hgMesh, char* file);

void hgCleanupTexture(HgTexture *t);

/*********
 * Light *
 *********/
typedef struct HgLight {
  vec3 position;
  vec3 color;
  vec3 ambient;
}HgLight;


/************
 * Renderer *
 ************/
void hgBeginDraw(void);

void hgDrawMesh(HgEntity *hgEntity, HgLight *hgLight, HgCamera *hgCamera);


/**************************************************************************/
                                #include "game.h"
/**************************************************************************/

#endif //HG_H
