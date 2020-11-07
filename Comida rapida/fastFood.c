#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#define EATING_TIME 2
#define CLEANING_TIME 1
#define CUSTOMERS 20
#define CHEFS 3
#define TABLES 10
#define FOOD_QUEUE_SIZE 10

sem_t foodQueueFull;
sem_t foodQueueEmpty;
sem_t cleanTables;
sem_t orderFood;
sem_t deliveredFood;
sem_t dirtyTables;

void initSemaphores();
//void destroySemaphores();
void* chefWork(void* args);
void* customerWork(void* args);
void* waiterWork(void* args);
void* cleanerWork(void* args);

int main() {
    pthread_t customers[CUSTOMERS], chefs[CHEFS], waiter, cleaner;
    int i;

    initSemaphores();

    for (i=0; i<CUSTOMERS; i++) {
        int* id = (int*) malloc(sizeof(int));
        *id = (i+1);
        pthread_create(&customers[i], NULL, customerWork, (void*) id);
    }
    for (i=0; i<CHEFS; i++) {
        int* id = (int*) malloc(sizeof(int));
        *id = (i+1);
        pthread_create(&chefs[i], NULL, chefWork, (void*) id);
    }
    pthread_create(&waiter, NULL, waiterWork, NULL);
    pthread_create(&cleaner, NULL, cleanerWork, NULL);

    for (i=0; i<CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }
    for (i=0; i<CHEFS; i++) {
        pthread_join(chefs[i], NULL);
    }
    pthread_join(waiter, NULL);
    pthread_join(cleaner, NULL);

//    destroySemaphores();
    return 0;
}

void initSemaphores() {
    sem_init(&foodQueueFull, 0, 0);
    sem_init(&foodQueueEmpty, 0, FOOD_QUEUE_SIZE);
    sem_init(&cleanTables, 0, TABLES);
    sem_init(&orderFood, 0, 0);
    sem_init(&deliveredFood, 0, 0);
    sem_init(&dirtyTables, 0, 0);
}

/*
void destroySemaphores() {
    sem_destroy(&foodQueueFull);
    sem_destroy(&foodQueueEmpty);
    sem_destroy(&cleanTables);
    sem_destroy(&orderFood);
    sem_destroy(&deliveredFood);
    sem_destroy(&dirtyTables);
}*/

void* chefWork(void* args) {
    int id = *((int*) args);
    while (1) {
        sem_wait(&foodQueueEmpty);
        printf("Soy el cocinero %d, preparando una comida\n", id);
        sem_post(&foodQueueFull);
    }
    pthread_exit(NULL);
}

void* customerWork(void* args) {
    int id = *((int*) args);
    while (1) {
        printf("Soy el cliente %d, estoy esperando una mesa\n", id);
        sem_wait(&cleanTables);
        printf("Soy el cliente %d, obtuve una mesa\n", id);
        sem_post(&orderFood);
        printf("Soy el cliente %d, pedi la comida\n", id);
        sem_wait(&deliveredFood);
        printf("Soy el cliente %d, me llegó la comida\n", id);
        sleep(EATING_TIME);
        printf("Soy el cliente %d, termine de comer, libero la mesa\n", id);
        sem_post(&dirtyTables);
    }
    pthread_exit(NULL);
}

void* waiterWork(void* args) {
    while (1) {
        sem_wait(&orderFood);
        printf("Soy el camarero, tomé un pedido de comida\n");
        sem_wait(&foodQueueFull);
        printf("Soy el camarero, saqué una comida de la cola de comidas\n");
        sem_post(&foodQueueEmpty);
        printf("Soy el camarero, entregué un pedido de comida\n");
        sem_post(&deliveredFood);
    }
    pthread_exit(NULL);
}

void* cleanerWork(void* args) {
    while (1) {
        printf("Soy el limpiador, estoy esperando a que se ensucie una mesa\n");
        sem_wait(&dirtyTables);
        printf("Soy el limpiador, hay una mesa para limpiar\n");
//        sleep(CLEANING_TIME);
        printf("Soy el limpiador, limpié una mesa\n");
        sem_post(&cleanTables);
    }
    pthread_exit(NULL);
}
