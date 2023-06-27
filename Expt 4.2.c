#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int copy_file(const char *src_path, const char *dst_path) {
    FILE *src_file, *dst_file;
    char buffer[4096];
    size_t bytes_read;
    if ((src_file = fopen(src_path, "rb")) == NULL) {
        return 0;
    }
    if ((dst_file = fopen(dst_path, "wb")) == NULL) {
        fclose(src_file);
        return 0;
    }
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes_read, dst_file);
    }
    fclose(src_file);
    fclose(dst_file);
    return 1;
}

int copy_dir(const char *src_path, const char *dst_path) {
    DIR *src_dir, *dst_dir;
    struct dirent *src_entry;
    struct stat src_stat;
    char src_item_path[1024], dst_item_path[1024];
    if ((src_dir = opendir(src_path)) == NULL) {
        return 0;
    }
    if ((dst_dir = opendir(dst_path)) == NULL) {
        mkdir(dst_path);
        dst_dir = opendir(dst_path);
    }
    while ((src_entry = readdir(src_dir)) != NULL) {
        if (strcmp(src_entry->d_name, ".") == 0 || strcmp(src_entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(src_item_path, sizeof(src_item_path), "%s/%s", src_path, src_entry->d_name);
        snprintf(dst_item_path, sizeof(dst_item_path), "%s/%s", dst_path, src_entry->d_name);
        if (lstat(src_item_path, &src_stat) != 0) {
            continue;
        }
        if (S_ISREG(src_stat.st_mode)) {
            copy_file(src_item_path, dst_item_path);
        } else if (S_ISDIR(src_stat.st_mode)) {
            copy_dir(src_item_path, dst_item_path);
        }
    }
    closedir(src_dir);
    closedir(dst_dir);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source directory> <destination directory>\n", argv[0]);
        return 1;
    }
    copy_dir(argv[1], argv[2]);
    printf("Contents of %s copied to %s\n", argv[1], argv[2]);
    return 0;
}
