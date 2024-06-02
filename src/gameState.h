/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

#ifndef GAEM_H
#define GAEM_H

struct HgGameState {
  
  HgCamera *camera;

  HgLight *light;
  
  HgEntity *hgSymbol;
  HgEntity *hgPlanet;
  HgMesh *hgSymbolMesh;
  HgMesh *hgPlanetMesh;

  float *rot;

};

#endif
