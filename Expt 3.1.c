#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFSIZE 512

int main() {
    int fd1, fd2, n;
    struct stat st;
    off_t size, offset;
    char buf[BUFSIZE];

    fd1 = open("text.txt", O_RDONLY);
    if (fd1 < 0) {
        perror("open");
        return 1;
    }

    fd2 = open("novel.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 < 0) {
        perror("open");
        return 1;
    }

    if (fstat(fd1, &st) < 0) {
        perror("fstat");
        return 1;
    }

    size = st.st_size;
    offset = size / 2;

    // lseek(fd1, offset, SEEK_SET); // For the second half of the file

    // while ((n = read(fd1, buf, 100)) > 0) {
    //     write(fd2, buf, n);
    // }

    while ((n = read(fd1, buf, offset)) > 0) {
        write(fd2, buf, n);
        break; // The data of length of offset i.e., half of data, is copied into the new file
        // Without the break statement, the sfull data is copied into the file.
    }

    close(fd1);
    close(fd2);

    return 0;
}
