#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFSIZE 512

int main() {
    int fd, n;
    fd = open("dollar.txt", O_WRONLY | O_CREAT, 4343);
    if(fd < 0) {
        perror("open");
        return 1;
    }
    char buf[BUFSIZ];
    while((n = read(0, buf, 1)) > 0) {
        // printf("%c", buf[0]); // Used to re-print the entered elements including the dollar sign
        if(buf[0] == '$')
            break;
        write(fd, buf, n);
    }
    close(fd);
}

// pid_t pid = getpid(); 
// pid_t pid = getpid().getppid(); // to get the parent process id
// pid_t child_pid = fork();