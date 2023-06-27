#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s n\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "fork error\n");
            return 1;
        }

        if (pid == 0) {
            printf("Child %d with parent %d\n", getpid(), getppid());
        }
    }

    // Wait for all child processes to terminate
    while (wait(NULL) > 0) {}

    return 0;
}