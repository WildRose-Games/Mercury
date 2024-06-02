/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

void hgInitEntity(HgEntity *entity, HgMesh *mesh){
  entity->mesh = mesh;
  glm_mat4_identity(entity->trans);
}
