/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

struct HgShader{
  uint32_t program;
};

struct HgVertexBuffer{
  uint32_t vbo;
  uint32_t ibo;
  uint32_t count;
};

struct HgTexture{
  uint32_t id;
  int width;
  int height;
  int bpp;
};

struct HgMesh{
  HgVertexBuffer vb;
  HgTexture t;
};

HgShader meshShader = {0};

#include "glError.c"
#include "glVertexBuffer.c"
#include "glTexture.c"
#include "glShader.c"
#include "glRenderer.c"

void hgCleanupMesh(HgArena *arena, HgMesh *mesh){
  hgCleanupVertexBuffer(&mesh->vb);
  hgCleanupTexture(&mesh->t);
  hgArenaPop(arena, mesh, sizeof(HgMesh));
}

int hgInitGL(void){

  // Get GL functions (SDL2)
  if (!gladLoadGLES2Loader(SDL_GL_GetProcAddress)) {
    HG_FATAL("Failed to load OpenGL function addresses!");
    return -1;
  }

  GL_CALL(glEnable(GL_DEPTH_TEST));
  GL_CALL(glDepthFunc(GL_LESS));  
  
  GL_CALL(glEnable(GL_BLEND));
  GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  return 0;
}
