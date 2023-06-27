#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
void *thread_function(void *args) {
    int i, j;
    int main() {
        pthread_t a_thread;
        pthread_create(&a_thread, NULL, thread_function, NULL);
        pthread_join(a_thread, NULL); // Using this, the thread is first executed and thek main is excecuted later
        // Commenting this line would make the program to execute the thread and main program over-lappingly.
        printf("Inside main program\n");
        for(j=20;j<25;j++) {
            printf("%d", j);
            sleep(1);
        }
    }
}
// This code executes thread and main program over-lappingly,
// but using thread, the thread is executed first and main is executed later
void *thread_function(void *arg) {
    printf("Inside thread\n");
    for(i=0;i<5;i++) {
        printf("%d\n", i);
        sleep(1);
    }
}