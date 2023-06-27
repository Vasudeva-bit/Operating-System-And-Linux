#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

int main() {
    char* desktop = getenv("HOME"); // Get the path to the desktop directory
    char directory[100] = {0};
    sprintf(directory, "%s/Desktop/MyDirectory", desktop); // Create a new directory on the desktop
    mkdir(directory);

    char filename[100] = {0};
    sprintf(filename, "%s/MyFile.txt", directory); // Create a new file inside the directory
    FILE* file = fopen(filename, "w");
    fprintf(file, "This is a test file.");
    fclose(file);

    DIR* dir = opendir(directory); // List the contents of the directory
    struct dirent* entry;
    printf("Contents of %s:\n", directory);
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);

    return 0;
}
