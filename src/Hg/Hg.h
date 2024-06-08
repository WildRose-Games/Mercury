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
 *  TABLE OF CONTENTS ---- (00.00)
 *  
 *  Usefull Macro Defines and Includes -------- (01.00)
 *    Standard Library Includes ----------- (01.01)
 *    Window Initial Size ----------------- (01.02)
 *    Max Log or Filepath Length ---------- (01.03)
 *    Math Functions (cglm) --------------- (01.04)
 *
 *  Mercury Engine ------------ (02.00)
 *    Input --------------- (02.01)
 *    File IO ------------- (02.02)
 *    Mesh ---------------- (02.03)
 *    Camera -------------- (02.04)
 *    Entity -------------- (02.05)
 *    Light --------------- (02.06)
 *    Load Wavefront obj -- (02.07)
 *    Shader -------------- (02.08)
 *    Vertex Buffer ------- (02.09)
 *    Textures ------------ (02.10)
 *    Rendering ----------- (02.11)
 *  
 *  game.h ---- (03.00)
 *
 */

/************************************************************
 * Use the number keys "(0X.XX)" in the table of contents   *
 * to find the section with a search                        *
 ************************************************************/

#ifndef HG_H
#define HG_H

/***************************************************************************
 *                                                                         *
 *  Basic helper macros and includes                              (01.00)  *
 *                                                                         *
 ***************************************************************************/


/*************************************
 * Standard Library Includes (01.01) *
 *************************************/
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

/*******************************
 * Window Initial Size (01.02) *
 *******************************/
#define INIT_WIDTH 640
#define INIT_HEIGHT 480

/**************************************
 * Max Log or Filepath Length (01.03) *
 **************************************/
#include "HgL_Log.h"
#define LOG_LENGTH 1028
#define PATH_LENGTH 512

/*memory allocation
 * TODO move this somewhere else, users should not need to modify this file.
 */
#include "HgL_Arena.h"
#define ALLOC_MEM_SIZE GIGABYTES(1)

/*********************************
 * Math Functions (cglm) (01.04) *
 *********************************/
#include <cglm/cglm.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif /* M_PI*/

#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

/* Returns number of bytes in an X_byte */
#define KILOBYTES(x) ((x) * 1024) 
#define MEGABYTES(x) (KILOBYTES(x) * 1024) 
#define GIGABYTES(x) ( (size_t) MEGABYTES(x) * 1024) 

/* Returns the max or min */
#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

/**************************************************************************
 *                                                                        *
 *  Mercury Game Engine                                          (02.00)  *
 *                                                                        *
 **************************************************************************/

/*****************
 * Input (02.01) *
 *****************/

/* Button press data. Can detect multiple presses in a frame */
typedef struct HgButtonInput{
  bool isStartDown; /* Did button start frame pressed? */
  bool isEndDown; /* Did button end frame pressed? */
  uint32_t count; /* How many times was the button pressed? */
}HgButtonInput;

/*  Analog stick data, captures the general motion since last frame*/
typedef struct HgStickInput{
  int startX;
  int startY;
  int endX;
  int endY;
}HgStickInput;

/* The total input data recieved since the last frame*/
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

/*******************
 * File IO (02.02) *
 *******************/

/* Get size of file in bytes */
size_t hgGetFileSize(
              const char* filepath /* filepath to check size of. */
                    );

/* Get entire contents of file (up to size) as char* string */
void hgGetFileStr(
  char* buffer,         /* pointer to char buffer */
  size_t size,          /* size of char buffer*/
  const char* filepath  /* filepath of file to get contents of */
);

/* Delete file at this location */
void hgDeleteFile(const char* filepath);

/* Copy file from fileSrc to fileDest filepaths */
void hgCopyFile(const char* fileSrc, const char* fileDest);

/* Returns the time since last modification of file */
time_t hgFileModTime(const char* filepath);

/****************
 * Mesh (02.03) *
 ****************/

/* Defied by platform, games only need pointers to meshes. */
typedef struct HgMesh HgMesh;

/* cleans up mesh memory on gpu */
void hgCleanupMesh(HgArena *hgArena, HgMesh *hgMesh);

/******************
 * Camera (02.04) *
 ******************/
typedef struct HgCamera {
  mat4 proj;
  mat4 view;
}HgCamera;

/******************
 * Entity (02.05) *
 ******************/
typedef struct HgEntity{
  mat4 trans;
  HgMesh *mesh;
  vec2 padding;
}HgEntity;

void hgInitEntity(HgEntity *hgEntity, HgMesh *hgMesh);

/*******************************
 * Lights and Lighting (02.06) *
 *******************************/
typedef struct HgLight {
  vec3 position;
  vec3 color;
  vec3 ambient;
}HgLight;

/******************************
 * Load Wavefront obj (02.07) *
 ******************************/

/* Creates a mesh from wavefront obj file and relevant mtl file. */
HgMesh* hgLoadObjMesh(
    HgArena *hgArena, /* Which arena to push the HgMesh struct onto */

    const char* fileName, /* name of obj file in res/modles folder
                             (without .obj at end) */

    const char* objectName /* Name of 'o' object in obj file 
                            * .obj files can have several */
);

/* If fileName and objectName are the same */
#define hgLoadMesh(a, b) hgLoadObjMesh(a, b, b)


/******************
 * Shader (02.08) *
 ******************/

/* Defined by platform. Games only need pointers to shaders. */
typedef struct HgShader HgShader;

/* Creates a shader from file */
HgShader hgCreateShader(
    HgArena *arena, /* Arena memory to open the file*/

    const char *shaderName /* Name of shader in res/shaders/ folder.
                              an example of "X" would load X.vert and X.frag */
);

/* Bind this shader to be used with a draw call */
void hgBindShader(HgShader *s);

/* Name of shaders to use for meshes */
#define MESH_SHADER_FILE "mesh"

/* Bind the shader used for meshes */
void hgBindMeshShader(void);

/* Unbind the shader */
void hgUnbindShader(void);

/* remove the shader from the gpu */
void hgCleanupShader(HgArena *hgArena, HgShader *s);

/* Long list of function declarations for passing uniform data to shaders.
   look at that file for the documentation */
#include "uniform.h"

/*************************
 * Vertex Buffer (02.09) *
 *************************/

/* VertexBuffer defined by platform. Games only need pointers to a vb */
typedef struct HgVertexBuffer HgVertexBuffer;

/* Data each vertex needs */
typedef struct HgVertex {
  vec3 position;  
  vec3 normal;
  vec2 texture;
}HgVertex;

/* Creates the vertexBuffer on a mesh. You don't have to do this manually,
 * in stead, look at "Load Wavefront obj" -- (02.07)*/
void hgCreateMeshVertexBuffer(
    HgMesh *hgMesh, /* ptr to mesh who's vertex buffer we are creating */

    HgVertex* data, /* buffer of HgVertex, model data */

    uint32_t vertCount, /* count of HgVertex in data buffer */

    uint16_t* inds, /* buffer of indicies, order of verticies */

    uint32_t indCount /* count of indices in inds */
);

/* Bind this vertex buffer for rendering geometry*/
void hgBindVertexBuffer(HgVertexBuffer *vb, HgShader *s);

/* Unbind the vertex buffer */
void hgUnbindVertexBuffer(void);

/* Delete vertex data on gpu */
void hgCleanupVertexBuffer(HgVertexBuffer *vb);

/********************
 * Textures (02.10) *
 ********************/

/* texture defined by platform. Games only need pointers to a texture */
typedef struct HgTexture HgTexture;

/* Bind this texture to render */
void hgBindTexture(HgTexture *t, uint32_t slot);

/* unbind texture */
void hgUnbindTexture(void);

/* load image in res/models/ as texture, and update mesh to use that texture*/
void hgLoadMeshTexture(HgMesh *hgMesh, char* fileName);

/* delete texture in gpu */
void hgCleanupTexture(HgTexture *t);

/*********************
 * Rendering (02.11) *
 *********************/

/* Start the process of drawing this frame (i.e, clear the frame?)*/
void hgBeginDraw(void);

/* Draw an entity, with a lightsource, from camera's perspective */
void hgDrawEntity(HgEntity *hgEntity, HgLight *hgLight, HgCamera *hgCamera);


/**************************************************************************/

    #include "game.h"                                          /* (03.00) */

/**************************************************************************/

#endif /* HG_H */
