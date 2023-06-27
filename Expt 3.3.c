#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    char buf;
    ssize_t bytes_read;
    while ((bytes_read = read(fd, &buf, 1)) > 0) {
        write(STDOUT_FILENO, &buf, bytes_read);
        // Here, STDOUT_FILENO can replaced by 1, that menas console out, while 0 means console input
    }

    if (bytes_read == -1) {
        perror("read");
        return 1;
    }

    close(fd);

    return 0;
}
