/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 *
 *  Purpose: Handles opengl shaders
 */

#include <alloca.h>

uint32_t hgCompileShader(uint32_t type, const char* src){
  GL_CALL(uint32_t id = glCreateShader(type));
  GL_CALL(glShaderSource(id, 1, &src, NULL));
  GL_CALL(glCompileShader(id));

  int result;
  GL_CALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
  if(result == GL_FALSE){
    int len;
    GL_CALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len));
    char *message =  (char*)alloca(sizeof(char) * len);
    GL_CALL(glGetShaderInfoLog(id, len, &len, message));
    HG_ERROR("Failed to compile %s shader:", 
        (type == GL_VERTEX_SHADER ? "vertex": "fragment"));
    printf("%s\n", message);
    GL_CALL(glDeleteShader(id));
    return 0;
  }
  return id;
}

HgShader hgCreateShader(HgArena *arena, const char *file){

  char vertFile[PATH_LENGTH];
  char fragFile[PATH_LENGTH];
  snprintf(vertFile, PATH_LENGTH, "res/shaders/%s.vert", file);
  snprintf(fragFile, PATH_LENGTH, "res/shaders/%s.frag", file);

  HgShader sp = {0};
  GL_CALL(sp.program = glCreateProgram());
 
  size_t vertSize = hgGetFileSize(vertFile);
  size_t fragSize = hgGetFileSize(fragFile);

  char *vertSrc = hgArenaPush(arena, vertSize);
  char *fragSrc = hgArenaPush(arena, fragSize);

  hgGetFileStr(vertSrc, vertSize, vertFile); 
  hgGetFileStr(fragSrc, fragSize, fragFile); 

  uint32_t vs = hgCompileShader(GL_VERTEX_SHADER, vertSrc);
  uint32_t fs = hgCompileShader(GL_FRAGMENT_SHADER, fragSrc);

  GL_CALL(glAttachShader(sp.program, vs));
  GL_CALL(glAttachShader(sp.program, fs));
  GL_CALL(glLinkProgram(sp.program));
  GL_CALL(glValidateProgram(sp.program));

  hgArenaPop(arena, fragSrc, fragSize);
  hgArenaPop(arena, vertSrc, vertSize);

  GL_CALL(glDeleteShader(vs));
  GL_CALL(glDeleteShader(fs));
  return sp;
}

void hgBindShader(HgShader *sp){
  GL_CALL(glUseProgram(sp->program));
}

void hgBindMeshShader(void){
  GL_CALL(glUseProgram(meshShader.program));
}

void hgUnBindShader(void){
  GL_CALL(glUseProgram(0));
}

void hgCleanupHgShader(HgArena *arena, HgShader *sp){
  hgArenaPop(arena, sp, sizeof(HgShader));
  GL_CALL(glDeleteProgram(sp->program));
}

int hgGetUniformLocation(HgShader *sp, const char *name){
  GL_CALL(int uLocation = glGetUniformLocation(sp->program, name));
  if(uLocation == -1){
    HG_WARN("Uniform %s does not exist in shader", name);
  }
  return uLocation;
}

void hgUniformFloat(HgShader *sp,
                  const char *name,
                  float data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform1f(uLocation, data));
}

void hgUniformVec2(HgShader *sp,
                 const char *name,
                 vec2 data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform2f(uLocation, data[0], data[1]));
}

void hgUniformVec3(HgShader *sp,
                 const char *name,
                 vec3 data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform3f(uLocation, data[0], data[1], data[2]));
}
void hgUniformVec4(HgShader *sp,
                 const char *name,
                 vec4 data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform4f(uLocation, data[0], data[1], data[2], data[3]));
}

void hgUniformInt(HgShader *sp,
                const char *name,
                int data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform1i(uLocation, data));
}

void hgUniformIvec2(HgShader *sp,
                  const char *name,
                  ivec2 data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform2i(uLocation, data[0], data[1]));
}

void hgUniformIvec3(HgShader *sp,
                  const char *name,
                  ivec3 data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform3i(uLocation, data[0], data[1], data[2]));
}

void hgUniformIvec4(HgShader *sp,
                  const char *name,
                  ivec4 data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform4i(uLocation, data[0], data[1], data[2], data[3]));
}

void hgUniformMat2(HgShader *sp,
                 const char *name,
                 bool isTrans,
                 mat2 data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniformMatrix2fv(uLocation, 1, isTrans, *data));
}
void hgUniformMat3(HgShader *sp,
                 const char *name,
                 bool isTrans,
                 mat3 data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniformMatrix3fv(uLocation, 1, isTrans, *data));
}

void hgUniformMat4(HgShader *sp,
                 const char *name,
                 bool isTrans,
                 mat4 data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniformMatrix4fv(uLocation, 1, isTrans, *data));
}

void hgUniformFloatv(HgShader *sp,
                  const char *name,
                  uint32_t count,
                  float *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform1fv(uLocation, count, data));
}

void hgUniformVec2v(HgShader *sp,
                 const char *name,
                 uint32_t count,
                 vec2 *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform2fv(uLocation, count, *data));
}

void hgUniformVec3v(HgShader *sp,
                 const char *name,
                 uint32_t count,
                 vec3 *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform3fv(uLocation, count, *data));
}
void hgUniformVec4v(HgShader *sp,
                 const char *name,
                 uint32_t count,
                 vec4 *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform4fv(uLocation, count, *data));
}

void hgUniformIntv(HgShader *sp,
                const char *name,
                uint32_t count,
                int *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform1iv(uLocation, count, data));
}

void hgUniformIvec2v(HgShader *sp,
                  const char *name,
                  uint32_t count,
                  ivec2 *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform2iv(uLocation, count, *data));
}

void hgUniformIvec3v(HgShader *sp,
                  const char *name,
                  uint32_t count,
                  ivec3 *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform3iv(uLocation, count, *data));
}

void hgUniformIvec4v(HgShader *sp,
                  const char *name,
                  uint32_t count,
                  ivec4 *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniform4iv(uLocation, count, *data));
}

void hgUniformMat2v(HgShader *sp,
                 const char *name,
                 uint32_t count,
                 bool isTrans,
                 mat2 *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniformMatrix2fv(uLocation, count, isTrans, **data));
}
void hgUniformMat3v(HgShader *sp,
                 const char *name,
                 uint32_t count,
                 bool isTrans,
                 mat3 *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniformMatrix3fv(uLocation, count, isTrans, **data));
}
void hgUniformMat4v(HgShader *sp,
                 const char *name,
                 uint32_t count,
                 bool isTrans,
                 mat4 *data){
  hgBindShader(sp);
  int uLocation = hgGetUniformLocation(sp, name); 
  GL_CALL(glUniformMatrix4fv(uLocation, count, isTrans, **data));
}
