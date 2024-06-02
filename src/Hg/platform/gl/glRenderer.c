/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

void hgBeginDraw(void){
  GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));  
}

void hgDrawMesh(HgEntity *entity, HgLight *light, HgCamera *camera){
  
  hgUniformVec3(&meshShader,"uAmbient", light->ambient);
  hgUniformVec3(&meshShader,"uLightPos", light->position);
  hgUniformVec3(&meshShader,"uLightColor", light->color);
  
  mat4 mvp = {0};

  glm_mat4_identity(mvp);
  glm_mat4_mul(camera->proj, camera->view, mvp);
  glm_mat4_mul(mvp, entity->trans, mvp);

  mat3 normMat = {0};

  glm_mat4_pick3(entity->trans, normMat);
  glm_mat3_inv(normMat, normMat);

  hgUniformMat4(&meshShader, "uMVP", false, mvp);
  hgUniformMat4(&meshShader, "uTrans", false, entity->trans);
  hgUniformMat3(&meshShader, "uNormMat", true, normMat);

  hgBindTexture(&entity->mesh->t, 0);
  hgUniformInt(&meshShader, "uTexture", 0);

  hgBindVertexBuffer(&entity->mesh->vb, &meshShader);

  GL_CALL(glDrawElements(GL_TRIANGLES,
                         entity->mesh->vb.count,
                         GL_UNSIGNED_SHORT, 
                         NULL));
}
