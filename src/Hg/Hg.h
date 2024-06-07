/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 *
 *  Purpose: Hg.h is the documentation for using the Mercury engine.
 *  It's also the header file for connecting the game translation unit, and
 *  the engine translation unit.
 */

/*
 *  TABLE OF CONTENTS     (00.00)
 *  
 *  Usefull Macro Defines and Includes      (01.00)
 *    Standard Library Includes           (01.01)
 *    Mercury Engine Version              (01.02)
 *    Shared Object (DLL) Filename Defs   (01.03)    
 *    Window Initial Size                 (01.04)
 *    Max Log or Filepath Length          (01.05)
 *    Math Functions (cglm)               (01.06)
 *
 *  Mercury Engine            (02.00)
 *    Input                 (02.01)
 *    File IO               (02.02)
 *    Mesh                  (02.03)
 *    Camera                (02.04)
 *    Entity                (02.05)
 *    Light                 (02.06)
 *    Load Wavefront obj    (02.07)
 *    Shader                (02.08)
 *    Vertex Buffer         (02.09)
 *    Textures              (02.10)
 *    Rendering             (02.11)
 *  
 *  game.h    (03.00)
 */

/*
 * Use the number keys "(0X.XX)" in the table of contents
 * to find the section with a search
 */

#ifndef HG_H
#define HG_H

/***************************************************************************
 *  Basic helper macros and includes                              (01.00)  *
 ***************************************************************************/


/* Standard Library Includes (01.01) */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

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

/* Window Initial Size (01.04) */
#define INIT_WIDTH 640
#define INIT_HEIGHT 480

/* Max Log or Filepath Length (01.05) */
#include "HgL_Log.h"
#define LOG_LENGTH 1028
#define PATH_LENGTH 512

/*memory allocation
 * TODO move this somewhere else, users should not need to modify this file.
 */
#include "HgL_Arena.h"
#define ALLOC_MEM_SIZE GIGABYTES(1)

/* Math Functions (cglm) (01.06) */
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
 *  Mercury Game Engine                                          (02.00)  *
 **************************************************************************/

/* Input (02.01) */
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

/* File IO (02.02) */
size_t hgGetFileSize(const char* fileLoc);

void hgGetFileStr(char* buffer, size_t size, const char* fileLoc);

void hgDeleteFile(const char* file);

void hgCopyFile(const char* src, const char* dest);

time_t hgFileModTime(const char* file);

/* Mesh (02.03) */
#define MESH_SHADER_FILE "mesh"

typedef struct HgMesh HgMesh;

void hgCleanupMesh(HgArena *hgArena, HgMesh *hgMesh);

/* Camera (02.04) */
typedef struct HgCamera {
  mat4 proj;
  mat4 view;
}HgCamera;

/* Entity (02.05) */
typedef struct HgEntity{
  mat4 trans;
  HgMesh *mesh;
  vec2 padding;
}HgEntity;

void hgInitEntity(HgEntity *hgEntity, HgMesh *hgMesh);

/* Lights and Lighting (02.06)*/
typedef struct HgLight {
  vec3 position;
  vec3 color;
  vec3 ambient;
}HgLight;

/* Load Wavefront obj (02.07) */
HgMesh* hgLoadObjMesh(HgArena *hgArena,
                      const char* file,
                      const char* object);

#define hgLoadMesh(a, b) hgLoadObjMesh(a, b, b)


/* Shader (02.08) */
typedef struct HgShader HgShader;

HgShader hgCreateShader(HgArena *hgArena, const char *file);

void hgBindShader(HgShader *s);

void hgBindMeshShader(void);

void hgUnbindShader(void);

void hgCleanupShader(HgArena *hgArena, HgShader *s);
#include "uniform.h"

/* Vertex Buffer (02.09) */
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

/* Textures (02.10) */
typedef struct HgTexture HgTexture;

void hgBindTexture(HgTexture *t, uint32_t slot);

void hgUnbindTexture(void);

void hgLoadMeshTexture(HgMesh *hgMesh, char* file);

void hgCleanupTexture(HgTexture *t);



/* Rendering (02.11) */
void hgBeginDraw(void);

void hgDrawMesh(HgEntity *hgEntity, HgLight *hgLight, HgCamera *hgCamera);


/**************************************************************************/
                                #include "game.h"
/***************************************************************(03.00)****/

#endif //HG_H
