#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {  // Child process
        printf("Child process with PID %d is exiting\n", getpid());
        exit(0);
    } else {  // Parent process
        sleep(2);
        printf("Parent process with PID %d is still running\n", getpid());
        wait(NULL);
        printf("Parent process with PID %d has finished waiting for child\n", getpid());
    }
    
    return 0;
}
