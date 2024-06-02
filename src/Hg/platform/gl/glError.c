/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 *
 *  Purpose: Handle error checking of opengl calls.
 */

void clearErrorGL(void){
  while(glGetError() != GL_NO_ERROR);
}

void checkErrorGL(char* file, int line){
  char code[30];
  GLenum err;
  while ((err = glGetError())){
    switch (err){
      case GL_INVALID_ENUM:
        snprintf(code, 30, "Invalid Enum");
        break;
      case GL_INVALID_VALUE:
        snprintf(code, 30, "Invalid Value");
        break;
      case GL_INVALID_OPERATION:
        snprintf(code, 30, "Invalid Operation.");
        break;
      case 0x0503:
        snprintf(code, 30, "Stack Overflow");
        break;
      case 0x0504:
        snprintf(code, 30, "Stack Underflow");
        break;
      case GL_OUT_OF_MEMORY:
        snprintf(code, 30, "Out of Memory");
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        snprintf(code, 30, "Invalid Framebuffer Operation");
        break;
      case 0x0507:
        snprintf(code, 30, "Context Lost");
        break;
      case 0x8031:
        snprintf(code, 30, "Table Too Large");
        break;
      default:
        snprintf(code, 30, "%04X", err);
        break;
    }
    printf("\033[0;34m[OPEN GL]\033[0;37m %s %d: \033[0;0m%s\n", file + 4, line, code); 
    isRunning = false;
  }
}

#ifdef DEBUG

#define GL_CALL(x) clearErrorGL(); x; checkErrorGL(__FILE__, __LINE__)  

#else

#define GL_CALL(x) x

#endif //DEBUG
