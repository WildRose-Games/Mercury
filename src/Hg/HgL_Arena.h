/*
 *  HGL_Arena - v0.1 - simple stb style implimentation 
 *                     of an arena/bump alocator, with
 *                     optional safety.
 *
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT (see bottom of file)
 *  No warranty implied; use at your own risk
 *
 *  TO CREATE THE IMPLEMENTATION:
 *  define HGL_ARENA_IMPLEMENTATION before including
 *  HgL_Arena.h
 *
 *      Example:
 *
 *      #include ...
 *      #include ...
 *      #define HGL_ARENA_IMPLEMENTATION
 *      #include "HgL_Arena.h"
 */

#ifndef HGL_ARENA_H
#define HGL_ARENA_H

#include <stdint.h>

typedef struct HgArena HgArena;

/*
 *  DOCUMENTATION:
 *
 *  What is an Arena Allocator:
 *
 *  an Arena Allocator is a basic and fast memory
 *  allocator. One large block of heap memory is allocated
 *  once, and you treat it like a stack, pushing memory to
 *  use and popping it back when you are done. 
 *  Memory must be returned in order.
 *  
 *  For more information:
 *
 *  https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator
 *
 *  Basic Usage:
 *  First you must create an HgArena:
 */  


HgArena* hgCreateArena(uint64_t initSize,
                         int memoryMultiple);

 /* 
 *    initSize: how large in bytes is this arena of memory
 *
 *    memoryMultiple : Arena Allocator will add padding
 *                   to make sure any allocation is a
 *                   multiple of this number 
 *                   (for memory alignment). 
 *
 *  Remember to destroy the HgArena when you are done with
 *  it!
 */
void hgDestroyArena(HgArena *hgArena);

/*
 *  To allocate some memory, simply push it:
 */

void* hgArenaPush(HgArena *hgArena, uint64_t allocSize);

/*
 *   You can also clear out that memory before getting it:
 */

void* hgArenaPushZero(HgArena *hgArena, uint64_t allocSize);

/*
 *  To "Free" the memory, just pop the pointer back to
 *  the arena with it's size
 *
 *  i.e:
 *  hgArenaPop(arena, memPtr, memSize);
 *  
 *  (This is a macro, so that HgL_Log will tell you the
 *  line in your program that caused the error,
 *  not the location of hgl_arena_popFunc)
 *
 *  both pointer and size are needed, because pointer is
 *  used for safety checking, but when checking is not 
 *  enabled, memory size is used.
 */
#include <stdio.h>
/* Include Hgl_Log.h before including this file for better logging */
#ifndef HG_ERROR
#define HG_ERROR(...) fprintf(stderr, ...);
#endif /* HG_ERROR */

#define  HGL_ARENA_NO_ALLOC 1
#define  HGL_ARENA_WRONG_PTR 2
#define  HGL_ARENA_WRONG_SIZE 3

#define hgArenaPop(a, r, s) do {                    \
  switch(hgl_arena_popFunc((a), (r), (s))){         \
    case(HGL_ARENA_NO_ALLOC):                       \
      HG_ERROR("No memory pushed in arena!");       \
      break;                                        \
    case(HGL_ARENA_WRONG_PTR):                      \
      HG_ERROR("Wrong pointer popped!");            \
      break;                                        \
    case(HGL_ARENA_WRONG_SIZE):                     \
      HG_ERROR("Wrong size with associated ptr!");  \
      break;                                        \
    default:                                        \
      break;                                        \
  }                                                 \
} while ( 0 )

int hgl_arena_popFunc(HgArena *arena,
                      void *returnMemory,
                      uint64_t returnSize);

/*
 *  You can also completely clear an arena to be used 
 *  again. Useful in gamedev to have memory that only
 *  lasts for the durration of the frame and is cleared at
 *  the end of the frame:
 */

void hgArenaPopAll(HgArena *hgArena);

/*
 *  END OF DOCUMENTATION
 */

#endif /* HGL_ARENA_H */

#ifdef HGL_ARENA_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>

struct HgArena {
  /* Data */
  uint8_t *dataMemory;
  uint64_t dataSize;
  uint64_t dataPosition;
  uint8_t dataMultiple;
  
#ifdef HG_BUILD_DEBUG
  /* Pointer check memory */
  uint64_t ptrSize;
  uint64_t ptrPosition;
  void** ptrMemory;
#endif /*HG_BUILD_DEBUG*/
};

HgArena* hgCreateArena(uint64_t initSize, int memMultiple){
  HgArena* arena = (HgArena*)malloc(sizeof(HgArena));
  arena->dataMultiple = memMultiple;
  if(arena == NULL){
    HG_ERROR("Out of Memory, Can't make new HgArena!");
    return NULL;
  }

  arena->dataSize = initSize;
  arena->dataPosition = 0;
  arena->dataMemory = (uint8_t*)malloc(arena->dataSize); 
  if(arena->dataMemory == NULL){
    HG_ERROR("Out of Memory, Can't make HgArena Data Memory!");
      free(arena);
      return NULL;
  }
  
#ifdef HG_BUILD_DEBUG
  arena->ptrSize = sizeof(void*) * 20;
  arena->ptrPosition = 0;
  arena->ptrMemory = (void**)malloc(arena->ptrSize); 
  if(arena->ptrMemory == NULL){
    HG_ERROR("Out of Memory, Can't make HgArena Pointer Memory!");
      free(arena);
      free(arena ->dataMemory);
      return NULL;
  }
#endif /*HG_BUILD_DEBUG*/
  return arena;
}

void hgDestroyArena(HgArena *arena){
 
  free(arena->dataMemory);
#ifdef HG_BUILD_DEBUG
  free(arena->ptrMemory);
#endif
  free(arena);
}

void* hgArenaPush(HgArena *arena, uint64_t allocSize){
  /* calc alignment */
  uint64_t allocPadded = allocSize;
  if(allocPadded % arena->dataMultiple != 0){
    allocPadded += arena->dataMultiple 
      - (allocPadded % arena->dataMultiple);
  }

  if(arena->dataPosition + allocPadded > arena->dataSize){
    HG_ERROR("Arena Alloc FAILED! Out of memory!");
    return NULL;
  }
#ifdef HG_BUILD_DEBUG
  if(arena->ptrPosition + sizeof(void*) > arena->ptrSize){
    void** newMemory = (void**)realloc(arena->ptrMemory,
                                       arena->ptrSize * 1.5);
    if(newMemory == NULL){
    HG_ERROR("Arena Alloc FAILED! Out of memory!");
    return NULL; 
    }
    arena->ptrMemory = newMemory;
  }
#endif /*HG_BUILD_DEBUG*/
  uint64_t oldPosition = arena->dataPosition;
  arena->dataPosition += allocPadded;
  void* ptr = arena->dataMemory + oldPosition;
 
#ifdef HG_BUILD_DEBUG
  arena->ptrMemory[arena->ptrPosition] = ptr;
  arena->ptrPosition++;
#endif /*HG_BUILD_DEBUG*/

  return ptr;

}

void* hgArenaPushZero(HgArena *arena, uint64_t allocSize){
  void* memory = hgArenaPush(arena, allocSize);
  memset(memory, 0, allocSize);
  return memory;
}

int hgl_arena_popFunc(HgArena *arena,
                      void* returnMemory,
                      uint64_t returnSize){

#ifdef HG_BUILD_DEBUG

  if(arena->ptrPosition == 0){
    return HGL_ARENA_NO_ALLOC;
  }

  void* peek = arena->ptrMemory[arena->ptrPosition - 1];
  if(returnMemory == peek){
    
    uint64_t dataMemSize = (uint64_t)arena->dataMemory
                    + (uint64_t)arena->dataPosition 
                    - (uint64_t)returnMemory;
    /* calc alignment */
    uint64_t returnPadded = returnSize;
    if(returnPadded % arena->dataMultiple != 0){
      returnPadded += arena->dataMultiple 
        - (returnPadded % arena->dataMultiple);
    }

    if(returnPadded != dataMemSize){
      return HGL_ARENA_WRONG_SIZE;
    }

    arena->dataPosition = (uint64_t)returnMemory 
                          - (uint64_t)arena->dataMemory;

    arena->ptrPosition--;
    return 0;
  }else{
    return HGL_ARENA_WRONG_PTR;
  }
#else

  /* calc alignment */
  uint64_t returnPadded = returnSize;
  if(returnPadded % arena->dataMultiple != 0){
    returnPadded += arena->dataMultiple 
      - (returnPadded % arena->dataMultiple);
  }

  if(arena->dataPosition < returnPadded){
    return HGL_ARENA_WRONG_SIZE;
  }

  arena->dataPosition -= returnPadded;

  return 0;

#endif /*HG_BUILD_DEBUG*/
}

void hgArenaPopAll(HgArena *arena){
  arena->dataPosition = 0;
}

#endif /* HGL_ARENA_IMPLEMENTATION */

/*
  LICENSE (MIT)

  Copyright (c) 2024 Gwenivere Benzschawel

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
