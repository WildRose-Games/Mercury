/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 *
 *  purpose: define functions associated with HgEntity struct
 */

void hgInitEntity(HgEntity *entity, HgMesh *mesh){
  entity->mesh = mesh;
  glm_mat4_identity(entity->trans);
}
