// run with following command
// gcc -pthread -o run "Multiple Semaphore.c"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 10
#define THREADS 4

int buffer[MAX];
int fill = 0;
int use = 0;

void put(int value) {
    buffer[fill] = value;
    fill = (fill + 1) % MAX;
}

int get() {
    int tmp = buffer[use];
    use = (use + 1) % MAX;
    return tmp;
}

sem_t empty[THREADS];
sem_t full[THREADS];

void *producer(void *arg) {
    int i;
    int id = *(int *)arg;
    for (i = 0; i < MAX * THREADS; i++) {
        sem_wait(&empty[id]);
        put(i);
        sem_post(&full[id]);
    }
}

void *consumer(void *arg) {
    int i;
    int id = *(int *)arg;
    for (i = 0; i < MAX * THREADS / 2; i++) {
        sem_wait(&full[id]);
        int tmp = get();
        sem_post(&empty[id]);
        printf("%d\n", tmp);
    }
}

int main(int argc, char *argv[]) {
    pthread_t threads[THREADS];
    int thread_args[THREADS];
    int i;
    for (i = 0; i < THREADS; i++) {
        sem_init(&empty[i], 0, MAX);
        sem_init(&full[i], 0, 0);
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, producer, &thread_args[i]);
    }
    for (i = 0; i < THREADS / 2; i++) {
        pthread_create(&threads[i + THREADS], NULL, consumer, &thread_args[i]);
    }
    for (i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}