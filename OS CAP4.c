#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock1;
pthread_mutex_t lock2;

void* worker(void* arg) {
    pthread_mutex_lock(&lock1);
    pthread_mutex_lock(&lock2);
    printf("Thread acquired resource\n");
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
    return NULL;
}

int main() {
    pthread_t threads[10];
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);

    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);

    return 0;
}