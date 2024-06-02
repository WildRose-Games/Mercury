/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 *
 *  Purpose: Handles model vertex data (vertex buffers in OpenGL)
 */

void setupAttrib(uint32_t index, int size, int position){
  GL_CALL(glEnableVertexAttribArray(index)); 
  GL_CALL(glVertexAttribPointer(index,
                                size,
                                GL_FLOAT,
                                GL_FALSE,
                                sizeof(HgVertex),
                                (const void*)(position * sizeof(float)))); 
}

void setupAllAttribs(HgShader *sp){
  int posLoc = glGetAttribLocation(sp->program,"aPosition");
  int normLoc = glGetAttribLocation(sp->program,"aNormal");
  int texLoc = glGetAttribLocation(sp->program,"aTexCoord");

  setupAttrib(posLoc, 3, 0);
  setupAttrib(normLoc, 3, 3);
  setupAttrib(texLoc, 2, 6);

}

void hgBindVertexBuffer(HgVertexBuffer *vb, HgShader *sp){
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vb->vbo));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb->ibo));
  
  setupAllAttribs(sp);
}

void hgUnbindVertexBuffer(void){
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void hgCreateMeshVertexBuffer(HgMesh *mesh,
                              HgVertex* data,
                              uint32_t vertCount,
                              uint32_t vertexSize,
                              uint16_t* inds,
                              uint32_t indCount){
  

  mesh->vb.ibo = 0;
  mesh->vb.vbo = 0;
  mesh->vb.count = indCount;

  GL_CALL(glGenBuffers(1, &mesh->vb.ibo));
  GL_CALL(glGenBuffers(1, &mesh->vb.vbo));

  hgBindVertexBuffer(&mesh->vb, &meshShader);

  GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       indCount * sizeof(uint16_t),
                       inds,
                       GL_STATIC_DRAW));

  GL_CALL(glBufferData(GL_ARRAY_BUFFER,
                       vertCount * vertexSize,
                       data, GL_STATIC_DRAW));

  hgUnbindVertexBuffer();
}

void hgCleanupVertexBuffer(HgVertexBuffer *vb){
  //hgArenaPop(arena, vb, sizeof(HgVertexBuffer));
  GL_CALL(glDeleteBuffers(1, &vb->vbo));
  GL_CALL(glDeleteBuffers(1, &vb->ibo));
}
