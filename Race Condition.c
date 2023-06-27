#include <stdio.h>
#include <pthread.h>

int counter = 0;

void* worker(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        counter++;
    }
    return NULL;
}

int main() {
    pthread_t threads[10];

    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Counter value: %d\n", counter);

    return 0;
}