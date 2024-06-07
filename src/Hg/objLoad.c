/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 *
 *  Purpose: Uses the platform/file to load obj wavefront files, and thier 
 *  associated mtl file, to generate a mesh.
 */

typedef struct HgObjFileCounts {
  uint16_t verts;
  uint16_t pos;
  uint16_t texs;
  uint16_t norms;
  uint32_t inds;
}HgObjFileCounts;

void hgGetMtlTexture(HgArena *arena,
                     HgMesh *mesh,
                     char* mtlFile,
                     char* useMtl){

  size_t mtlSize = hgGetFileSize(mtlFile);
  char* mtlBuffer = hgArenaPush(arena, mtlSize);
  hgGetFileStr(mtlBuffer, mtlSize, mtlFile);

  bool isMtlFound = false;
  char* word = strtok(mtlBuffer, "\n ");
  while(word){

    if(strcmp(word, "newmtl") == 0){
      word = strtok(NULL, "\n ");
      if(strcmp(word, useMtl) == 0){
        isMtlFound = true;
        word = strtok(NULL, "\n ");
      }
    }
   
    if(isMtlFound){
      if(strcmp(word, "map_Kd") == 0){
        word = strtok(NULL, "\n ");
        hgLoadMeshTexture(mesh, word);
        break;
      }
    }
    word = strtok(NULL, "\n ");
  }
  
  hgArenaPop(arena, mtlBuffer, mtlSize);  
}

void hgObjGetCount(HgArena *arena,
                   const char* file,
                   HgObjFileCounts *counts){
  counts->verts = 0;
  counts->pos = 0;
  counts->texs = 0;
  counts->norms = 0;
  counts->inds = 0;

  char objFile[PATH_LENGTH];
  snprintf(objFile, PATH_LENGTH, "res/models/%s.obj", file);
  size_t objSize = hgGetFileSize(objFile);
  char* objBuffer = hgArenaPush(arena, objSize);
  hgGetFileStr(objBuffer, objSize, objFile);
  
  char* line = strtok(objBuffer, "\n");
  while(line){
    if(strncmp(line, "v ", 2) == 0){
      counts->pos++;
    }
    if(strncmp(line, "vt ", 3) == 0){
      counts->texs++;
    }
    if(strncmp(line, "vn ", 3) == 0){
      counts->norms++;
    }
    if(strncmp(line, "f ", 2) == 0){
      counts->inds += 3;
    }

    line = strtok(NULL, "\n");
  }//line

  counts->verts = counts->inds;

  if(counts->verts > 65535){
    HG_WARN("Model %s has too many verts for uint16_t", objFile);
  }
  hgArenaPop(arena, objBuffer, objSize);
}

HgMesh* hgLoadObjMesh(HgArena *arena,
                      const char* file,
                      const char* object){

  HgMesh *mesh = hgArenaPush(arena, sizeof(HgMesh));

  HgObjFileCounts counts = {0};
  hgObjGetCount(arena, file, &counts);
  
  uint16_t vertIndex = 0;
  uint16_t posIndex = 0;
  uint16_t texIndex = 0;
  uint16_t normIndex = 0;
  uint32_t indsIndex = 0;

  vec3 *v = hgArenaPush(arena, counts.verts * sizeof(vec3));
  vec2 *vt = hgArenaPush(arena, counts.texs * sizeof(vec2));
  vec3 *vn = hgArenaPush(arena, counts.norms * sizeof(vec3));
  HgVertex *verts = hgArenaPush(arena, counts.verts * sizeof(HgVertex));
  uint16_t *inds = hgArenaPush(arena, counts.inds * sizeof(uint16_t));

  char objFile[PATH_LENGTH] = {0};
  char mtlFile[PATH_LENGTH] = {0};
  char usedMtl[PATH_LENGTH] = {0};

  snprintf(objFile, PATH_LENGTH, "res/models/%s.obj", file);
  size_t objSize = hgGetFileSize(objFile);
  char* objBuffer = hgArenaPush(arena, objSize);
  hgGetFileStr(objBuffer, objSize, objFile);

  bool objFound = false;
  char* word = strtok(objBuffer, "\n /");
  while(word != NULL){
    if(objFound){
      // object
      if(strcmp(word, "o") == 0){
        break;
      }
      //mtllib
      if(strcmp(word, "mtllib") == 0){
        word = strtok(NULL, "\n /");
        snprintf(mtlFile, PATH_LENGTH, "res/models/%s", word); 
      }
      //usemtl
      if(strcmp(word, "usemtl") == 0){
        word = strtok(NULL, "\n /");
        snprintf(usedMtl, PATH_LENGTH, "%s", word); 
      }

      //vertex
      if(strcmp(word, "v") == 0){
        for(int i = 0; i < 3; i++){
          word = strtok(NULL, "\n /");
          v[posIndex][i] = atof(word);
        }
        posIndex++;
      }
      //texture
      if(strcmp(word, "vt") == 0){
        for(int i = 0; i < 2; i++){
          word = strtok(NULL, "\n /");
          vt[texIndex][i] = atof(word);
        }
        texIndex++;
      }
      //norm
      if(strcmp(word, "vn") == 0){
        for(int i = 0; i < 3; i++){
          word = strtok(NULL, "\n /");
          vn[normIndex][i] = atof(word);
        }
        normIndex++;
      }
      if(strcmp(word, "vp") == 0){
        HG_WARN("vp not supported in obj parsing yet");
      }
      //face
      if(strcmp(word, "f") == 0){
        //Only tris supported
        for(int i = 0; i < 3; i++){
          word = strtok(NULL, "\n /");
          int posInd = atoi(word) - 1; 
          inds[indsIndex++] =  vertIndex;
          verts[vertIndex].position[0] = v[posInd][0];
          verts[vertIndex].position[1] = v[posInd][1];
          verts[vertIndex].position[2] = v[posInd][2];

          if(counts.texs > 0){
            word = strtok(NULL, "\n /");
            int texInd = atoi(word) - 1;
            verts[vertIndex].texture[0] = vt[texInd][0];
            verts[vertIndex].texture[1] = vt[texInd][1];
          }

          if(counts.norms > 0){
            word = strtok(NULL, "\n /");
            int normInd = atoi(word) - 1;
            verts[vertIndex].normal[0] = vn[normInd][0];
            verts[vertIndex].normal[1] = vn[normInd][1];
            verts[vertIndex].normal[2] = vn[normInd][2];

          }
          vertIndex++;
        }
      }

    }else{ //find object
      if(strcmp(word, "mtllib") == 0){
        word = strtok(NULL, "\n /");
        snprintf(mtlFile, PATH_LENGTH, "res/models/%s", word); 
      }
      if(strcmp(word, "o") == 0){
        word = strtok(NULL, "\n /");
        if(strcmp(word, object) == 0){
          objFound = true;
        }
      }
    }
    word = strtok(NULL, "\n /");
  }

  if(meshShader.program == 0){
    meshShader = hgCreateShader(arena, MESH_SHADER_FILE);
  }
  
  hgCreateMeshVertexBuffer(mesh,
                           verts,
                           counts.verts,
                           inds,
                           counts.inds
                           ); 
  
  hgArenaPop(arena, objBuffer, objSize);
  hgArenaPop(arena, inds, counts.inds * sizeof(uint16_t));
  hgArenaPop(arena, verts, counts.verts * sizeof(HgVertex));
  hgArenaPop(arena, vn, counts.norms * sizeof(vec3));
  hgArenaPop(arena, vt, counts.texs * sizeof(vec2));
  hgArenaPop(arena, v, counts.verts * sizeof(vec3));

  hgGetMtlTexture(arena, mesh, mtlFile, usedMtl);

  return mesh;
}
