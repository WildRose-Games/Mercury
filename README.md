# Mercury Game Engine
![animation ofmercury symbol rotating](https://i.imgur.com/MvytJxf.gif)

My game engine, witten in C. Currently uses SDL2 and cglm.

The engine and the game are both Single Translation Unit Builds (STUB).
In debug builds, the game is dynamically linked with the engine for hot reloading game code.
In release builds, the game and engine are statically linked for better compiler optimizations.

## Build
(Currently only Unix style OS are supported)
requires make. for debug builds:
```
make
```
for release builds:
```
make release
```
output is in bin folder.
