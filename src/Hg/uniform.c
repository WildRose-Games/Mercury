/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

void hgUniformFloat(HgShader *s,
                  const char *name,
                  float data);

void hgUniformVec2(HgShader *s,
                 const char *name,
                 vec2 data);

void hgUniformVec2(HgShader *s,
                 const char *name,
                 vec2 data);

void hgUniformVec4(HgShader *s,
                 const char *name,
                 vec4 data);

void hgUniformInt(HgShader *s,
                const char *name,
                int data);

void hgUniformIvec2(HgShader *s,
                  const char *name,
                  ivec2 data);

void hgUniformIvec3(HgShader *s,
                  const char *name,
                  ivec3 data);

void hgUniformIvec4(HgShader *s,
                  const char *name,
                  ivec4 data);

void hgUniformMat2(HgShader *s,
                 const char *name,
                 bool isTrans,
                 mat2 data);

void hgUniformMat3(HgShader *s,
                 const char *name,
                 bool isTrans,
                 mat3 data);

void hgUniformMat4(HgShader *s,
                 const char *name,
                 bool isTrans,
                 mat4 data);

void hgUniformFloatv(HgShader *s,
                  const char *name,
                  uint32_t count,
                  float *data);

void hgUniformVec2v(HgShader *s,
                 const char *name,
                 uint32_t count,
                 vec2 *data);

void hgUniformVec3v(HgShader *s,
                 const char *name,
                 uint32_t count,
                 vec3 *data);

void hgUniformVec4v(HgShader *s,
                 const char *name,
                 uint32_t count,
                 vec4 *data);

void hgUniformIntv(HgShader *s,
                const char *name,
                uint32_t count,
                int *data);

void hgUniformIvec2v(HgShader *s,
                  const char *name,
                  uint32_t count,
                  ivec2 *data);

void hgUniformIvec3v(HgShader *s,
                  const char *name,
                  uint32_t count,
                  ivec3 *data);

void hgUniformIvec4v(HgShader *s,
                  const char *name,
                  uint32_t count,
                  ivec4 *data);

void hgUniformMat2v(HgShader *s,
                 const char *name,
                 uint32_t count,
                 bool isTrans,
                 mat2 *data);

void hgUniformMat3v(HgShader *s,
                 const char *name,
                 uint32_t count,
                 bool isTrans,
                 mat3 *data);

void hgUniformMat4v(HgShader *s,
                 const char *name,
                 uint32_t count,
                 bool isTrans,
                 mat4 *data);
