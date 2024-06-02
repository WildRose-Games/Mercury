/*
 *  Author: Gwenivere Benzschawel
 *  Copyright: 2024
 *  License: MIT
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

//POSIX
#include <sys/stat.h>

size_t hgGetFileSize(const char* fileLoc){
  FILE *fp;
  size_t size = 0;
    
  fp = fopen(fileLoc, "rb");
  if(fp == NULL) {
    HG_ERROR("Failed to load file: %s", fileLoc);
    return 0;
  }
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp)+1;
  fclose(fp);
  return size;
}

void hgGetFileStr(char* buffer, size_t size, const char* fileLoc){
  
  FILE *fp = fopen(fileLoc, "r");
  if(fp == NULL) {
    HG_ERROR("Failed to load file: %s", fileLoc);
    return;
  }
  memset(buffer, '\0', size);
  fread(buffer, 1, size-1, fp);
  fclose(fp);
}

void hgDeleteFile(const char* file){
  FILE *fp = NULL;
  fp = fopen(file, "r");
  if(fp == NULL){
    return;
  }
  fclose(fp); 
  remove(file);
}

void hgCopyFile(const char* src, const char* dest){
  FILE *srcFp = NULL, *destFp = NULL; 

  srcFp = fopen(src, "r"); 
  if(srcFp == NULL){
    HG_ERROR("Failed to open %s", src);
    return;
  }
  
  destFp = fopen(dest, "w");
  if(destFp == NULL){
    HG_ERROR("Failed to open %s", dest);
    fclose(srcFp);
    return;
  }

  int ch = 0;
  while( (ch = fgetc(srcFp)) != EOF ){
    fputc(ch, destFp);
  }

  fclose(srcFp);
  fclose(destFp);

}

time_t hgFileModTime(const char* file){
  struct stat fileStat;
  stat(file, &fileStat);
  return fileStat.st_ctime;
}
