/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

#include "Hg/Hg.h"

#include "gameState.h"

#include <stdlib.h>

HgVersionInfo hgGetVersionInfo(void){
  HgVersionInfo vi = {
    .major = 0,
    .minor = 0,
    .patch = 1,
    .gameName = "Hg_Sandbox"
  };
  return vi;
}

void hgStartGame(HgArena *arena, HgGameState *gs){

  gs->hgSymbol = hgArenaPush(arena, sizeof(HgEntity));
  gs->hgPlanet = hgArenaPush(arena, sizeof(HgEntity));
  gs->camera = hgArenaPush(arena, sizeof(HgCamera));
  gs->rot = hgArenaPush(arena, sizeof(float));
  gs->light = hgArenaPush(arena, sizeof(HgLight));

  gs->light->ambient[0] = 0.1;
  gs->light->ambient[1] = 0.1;
  gs->light->ambient[2] = 0.1;

  gs->light->position[0] = 2.0;
  gs->light->position[1] = 0.0;
  gs->light->position[2] = -0.1;

  gs->light->color[0] = 1.0;
  gs->light->color[1] = 1.0;
  gs->light->color[2] = 1.0;

  glm_perspective(CAMERA_FOV,
                  INIT_ASPECT_RATIO,
                  CAMERA_NEAR,
                  CAMERA_FAR,
                  gs->camera->proj);
  
  vec3 cameraPos = {0.0, 0.0, -2.0};
  glm_translate_make(gs->camera->view, cameraPos);

  //gs->mesh = loadMesh(arena, "Suzanne");
  gs->hgSymbolMesh =  hgLoadMesh(arena, "HgSymbol");
  gs->hgPlanetMesh =  hgLoadMesh(arena, "HgPlanet");
  //hgLoadMesh(arena, "HgSymbol");
  
  hgInitEntity(gs->hgSymbol, gs->hgSymbolMesh);
  hgInitEntity(gs->hgPlanet, gs->hgPlanetMesh);
}

void hgGameLoop(HgArena *arena,
                HgGameState *gs,
                HgInput *input,
                double delta){

  (void)(arena);

  *gs->rot = 0;
  *gs->rot += input->right.isEndDown ? -3.0 : 0.0;
  *gs->rot += input->left.isEndDown ? 3.0 : 0.0;
  glm_rotate_z(gs->hgSymbol->trans, *gs->rot * delta, gs->hgSymbol->trans);
  glm_rotate_y(gs->hgSymbol->trans, 1.0 * delta, gs->hgSymbol->trans);
  glm_rotate_y(gs->hgPlanet->trans, -1.0 * delta, gs->hgPlanet->trans);


  hgBeginDraw();   
  hgBindMeshShader();
  hgDrawMesh(gs->hgSymbol, gs->light, gs->camera);
  hgDrawMesh(gs->hgPlanet, gs->light, gs->camera);
}

void hgEndGame(HgArena *arena, HgGameState *gs){
  hgCleanupMesh(arena, gs->hgPlanetMesh);
  hgCleanupMesh(arena, gs->hgSymbolMesh);
}
