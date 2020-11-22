#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "./Table/table.h"
#define CUSTOMERS 50
#define CHEFS 3
#define TABLES 30
#define FOOD_QUEUE_SIZE 10

sem_t foodQueueFull;
sem_t foodQueueEmpty;
sem_t cleanTables;
sem_t orderFood;
sem_t deliveredFood;
sem_t dirtyTables;

void threads();
void initSemaphores();
void destroySemaphores();
void* chefWork(void* args);
void* customerWork(void* args);
void* waiterWork(void* args);
void* cleanerWork(void* args);

int main() {
    initSemaphores();
    tableHeader();
    threads();
    destroySemaphores();
    return 0;
}

void threads() {
    pthread_t customerThreads[CUSTOMERS], chefThreads[CHEFS], waiterThread, cleanerThread;
    int i;
    for (i=0; i<CUSTOMERS; i++) {
        int* id = (int*) malloc(sizeof(int));
        *id = (i+1);
        pthread_create(&customerThreads[i], NULL, customerWork, (void*) id);
    }
    for (i=0; i<CHEFS; i++) {
        int* id = (int*) malloc(sizeof(int));
        *id = (i+1);
        pthread_create(&chefThreads[i], NULL, chefWork, (void*) id);
    }
    pthread_create(&waiterThread, NULL, waiterWork, NULL);
    pthread_create(&cleanerThread, NULL, cleanerWork, NULL);


    for (i=0; i<CUSTOMERS; i++) {
        pthread_join(customerThreads[i], NULL);
    }
    pthread_join(waiterThread, NULL);
    pthread_join(cleanerThread, NULL);
    for (i=0; i<CHEFS; i++) {
        pthread_join(chefThreads[i], NULL);
    }
}

void initSemaphores() {
    sem_init(&foodQueueFull, 0, 0);
    sem_init(&foodQueueEmpty, 0, FOOD_QUEUE_SIZE);
    sem_init(&cleanTables, 0, TABLES);
    sem_init(&orderFood, 0, 0);
    sem_init(&deliveredFood, 0, 0);
    sem_init(&dirtyTables, 0, 0);
}

void destroySemaphores() {
    sem_destroy(&foodQueueFull);
    sem_destroy(&foodQueueEmpty);
    sem_destroy(&cleanTables);
    sem_destroy(&orderFood);
    sem_destroy(&deliveredFood);
    sem_destroy(&dirtyTables);
}

/*
 * ALGORITMO COCINERO:
    - repetir:
        - espero que se libere un espacio de comida -> wait(foodQueueEmpty)
        - preparo comida
        - brindo una nueva comida a la cola -> signal(foodQueueFull)
 */
void* chefWork(void* args) {
    int* id = ((int*) args);
    int columnNumber = 1;
    char text[COLUMN_SIZE];
    sprintf(text, "%d prepara comida", *id);
    while (1) {
        sem_wait(&foodQueueEmpty);
        tableRow(columnNumber, text);
        sem_post(&foodQueueFull);
    }
    free(id);
    pthread_exit(NULL);
}

/*
 * ALGORITMO CLIENTE:
    - repetir:
        - espero mesa limpia disponible -> wait(cleanTables)
        - ir a mesa
        - pedir comida -> signal(orderFood)
        - esperar comida -> wait(deliveredFood)
        - comer
        - libero la mesa (es decir, deja una mesa sucia) -> signal(dirtyTables)
 */
void* customerWork(void* args) {
    int *id = ((int*) args);
    int columnNumber = 0;
    char text1[COLUMN_SIZE];
    char text2[COLUMN_SIZE];
    char text3[COLUMN_SIZE];
    char text4[COLUMN_SIZE];
    char text5[COLUMN_SIZE];
    sprintf(text1, "%d espera mesa", *id);
    sprintf(text2, "%d obtuvo mesa", *id);
    sprintf(text3, "%d pidio comida", *id);
    sprintf(text4, "%d obtuvo comida", *id);
    sprintf(text5, "%d comio y se va", *id);
    while (1) {
        tableRow(columnNumber, text1);
        sem_wait(&cleanTables);
        tableRow(columnNumber, text2);
        sem_post(&orderFood);
        tableRow(columnNumber, text3);
        sem_wait(&deliveredFood);
        tableRow(columnNumber, text4);
        tableRow(columnNumber, text5);
        sem_post(&dirtyTables);
    }

    free(id);
    pthread_exit(NULL);
}

/*
 * ALGORITMO CAMARERO:
    - repetir:
        - espero a que haya un pedido -> wait(orderFood)
        - espero a que haya una comida -> wait(foodQueueFull)
        - saco una comida de la cola de comidas -> signal(foodQueueEmpty)
        - llevo la comida a un cliente -> signal(deliveredFood)
 */
void* waiterWork(void* args) {
    int columnNumber = 2;
    char text1[] = "Tome un pedido";
    char text2[] = "Saque una comida";
    char text3[] = "Entregue pedido";

    while (1) {
        sem_wait(&orderFood);
        tableRow(columnNumber, text1);
        sem_wait(&foodQueueFull);
        tableRow(columnNumber, text2);
        sem_post(&foodQueueEmpty);
        tableRow(columnNumber, text3);
        sem_post(&deliveredFood);
    }
    pthread_exit(NULL);
}

/*
 * ALGORITMO LIMPIADOR:
    - repetir:
        - espero a que haya una mesa sucia -> wait(dirtyTables)
        - limpio mesa mesa
        - nueva mesa limpia libre -> signal(cleanTables)
 */
void* cleanerWork(void* args) {
    int columnNumber = 3;
    char text1[] = "Inactivo";
    char text2[] = "Limpiando mesa";
    char text3[] = "Ya limpie";
    char text4[] = "Me voy";

    while (1) {
        tableRow(columnNumber, text1);
        sem_wait(&dirtyTables);
        tableRow(columnNumber, text2);
        tableRow(columnNumber, text3);
        sem_post(&cleanTables);
    }
    tableRow(columnNumber, text4);
    pthread_exit(NULL);
}
