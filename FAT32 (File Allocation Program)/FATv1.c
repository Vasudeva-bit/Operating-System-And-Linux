#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void bubbleSort(int arr[], int n) {
    int i, j;
    for (i = 0; i < n-1; i++)
        for (j = 0; j < n-i-1; j++)
            if (arr[j] > arr[j+1]) {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
}

int main() {
    int n, i;
    printf("Enter the number of integers: ");
    scanf("%d", &n);
    int arr[n];
    printf("Enter the integers:\n");
    for (i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
    pid_t pid = fork();
    if (pid == -1) {
        printf("Fork failed\n");
        return 1;
    }
    else if (pid == 0) { // child process
        printf("Sorting in child process...\n");
        bubbleSort(arr, n);
        printf("Child process sorted the integers.\n");
        // sleep(10); Uncommenting this line makes the child an orphan by terminating the parent before it's child
        exit(0);
    }
    else { // parent process
        printf("Sorting in parent process...\n");
        bubbleSort(arr, n);
        printf("Parent process sorted the integers.\n");
        wait(NULL); // wait for child process to finish
        /* Commenting the above line causes creation of zombie process, since the child process remains
        to be in the process table even after finishing since, the parent couldn't recieve the child's
        exit using wait command */
        printf("Child process has finished.\n");
        exit(0);
    }
    return 0;
}
