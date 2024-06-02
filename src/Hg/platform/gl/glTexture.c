/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

void hgBindTexture(HgTexture *t, uint32_t slot){
  GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, t->id));
}

void hgUnbindTexture(void){
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void hgLoadMeshTexture(HgMesh *mesh, char *file){
  uint8_t *buffer;

  char path[PATH_LENGTH];
  snprintf(path, PATH_LENGTH, "res/models/%s", file);

  stbi_set_flip_vertically_on_load(1);
  buffer = stbi_load(path, &mesh->t.width, &mesh->t.height, &mesh->t.bpp, 4);

  if(!buffer){
    HG_ERROR("Failed to load Texture: %s", file);
    return ; 
  }


  GL_CALL(glGenTextures(1, &mesh->t.id));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, mesh->t.id));

  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

  GL_CALL(glTexImage2D(GL_TEXTURE_2D, 
                       0,
                       GL_RGBA, 
                       mesh->t.width,
                       mesh->t.height,
                       0,
                       GL_RGBA,
                       GL_UNSIGNED_BYTE,
                       buffer));

  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
  
  stbi_image_free(buffer);

}

void hgCleanupTexture(HgTexture *t){
  GL_CALL(glDeleteTextures(1, &t->id)); 
}
