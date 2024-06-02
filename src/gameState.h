/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 *
 *  Purpose: You define the static memory that persists between frames here
 */

#ifndef HG_GAEM_H
#define HG_GAEM_H

struct HgGameState {
 
  /* Add whatever thing you need here */
  HgCamera *camera;

  HgLight *light;
  
  HgEntity *hgSymbol;
  HgEntity *hgPlanet;
  HgMesh *hgSymbolMesh;
  HgMesh *hgPlanetMesh;

  float *rot;

};

#endif /*HG_GAME_H*/
