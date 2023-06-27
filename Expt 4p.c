#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
int main() {
  DIR *dir;
  struct dirent *dirptr;
  int b = mkdir("Directory");
  dir = opendir("Directory");
  while(NULL != (dirptr = readdir(dir))) {
    printf("%s\n", dirptr->d_name);
    printf("%d\n", dirptr->d_type);
  }
  return 0;
}