#include <stdio.h>
#include <pthread.h>

void *thread_func(void *arg) {
    printf("Hello from thread!\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, thread_func, NULL);
    if (ret != 0) {
        printf("Error: pthread_create failed!\n");
        return 1;
    }
    printf("Created thread with ID %lu\n", tid);
    pthread_join(tid, NULL);
    return 0;
}
