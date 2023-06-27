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
        for (i = 0; i < n; i++) {
            printf("%d\n", arr[i]);
        }
        exit(0);
    }
    else { // parent process
        bubbleSort(arr, n); // sort the array

        char *args[n+2]; // array of arguments for execvp
        args[0] = "sort_child"; // name of the child process executable
        for (i = 0; i < n; i++) {
            char str[10];
            sprintf(str, "%d", arr[i]); // convert int to string
            args[i+1] = str;
        }
        args[n+1] = NULL; // terminate the argument list
        execvp(args[0], args); // execute the child process with arguments

        wait(NULL); // wait for child process to finish
        printf("Child process has finished.\n");
        for (i = 0; i < n; i++) {
            printf("%d\n", arr[i]);
        }
        printf("Parent process has finished.\n");
        exit(0);
    }
    printf("The End!");
    return 0;
}
