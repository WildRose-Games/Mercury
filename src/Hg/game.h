/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 *
 *  Purpose: If you want to make a game in Mercury, you need to impliment 
 *  these functions:
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

// return the version info and name of your game
HgVersionInfo hgGetVersionInfo(void);

// Struct of static memory that exists inbetween frames.
// You define this struct in gameState.h so that
// both the game and engine knows this. (engine needs size of this struct)
typedef struct HgGameState HgGameState;

// Runs once at the start of the game, to set up memory for the game loop.
void hgStartGame(HgArena *hgArena, HgGameState *gs);

// Runs once per frame. 
void hgGameLoop(HgArena *hgArena,
                HgGameState *gs,
                HgInput *input, 
                double delta);

// Called oncd at the end of the game to clean up.
void hgEndGame(HgArena *hgArena, HgGameState *gs);

#endif //HG_GAME_H
