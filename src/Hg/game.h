/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

#ifndef HG_GAME_H
#define HG_GAME_H

/*
 * What you need to impliment to make a game in Mercury:
 */

typedef struct HgVersionInfo {
  uint32_t major;
  uint32_t minor;
  uint32_t patch;
  const char* gameName;
}HgVersionInfo;

HgVersionInfo hgGetVersionInfo(void);

void hgStartGame(HgArena *hgArena, HgGameState *gs);

void hgGameLoop(HgArena *hgArena,
                HgGameState *gs,
                HgInput *input, 
                double delta);

void hgEndGame(HgArena *hgArena, HgGameState *gs);

#endif //HG_GAME_H
