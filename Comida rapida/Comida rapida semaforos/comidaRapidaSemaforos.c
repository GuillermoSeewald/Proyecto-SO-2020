#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "../Table/table.h"
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
    char cocinando[COLUMN_SIZE];
    sprintf(cocinando, "%d prepara comida", *id);
    while (1) {
        sem_wait(&foodQueueEmpty);
        tableRow(columnNumber, cocinando); // Metodo para imprimir en forma de tabla.
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
    char esperoMesa[COLUMN_SIZE];
    char obtuveMesa[COLUMN_SIZE];
    char pidoComida[COLUMN_SIZE];
    char obtuveComida[COLUMN_SIZE];
    char comi[COLUMN_SIZE];
    sprintf(esperoMesa, "%d espera mesa", *id);
    sprintf(obtuveMesa, "%d obtuvo mesa", *id);
    sprintf(pidoComida, "%d pidio comida", *id);
    sprintf(obtuveComida, "%d obtuvo comida", *id);
    sprintf(comi, "%d comio y se va", *id);
    while (1) {
        tableRow(columnNumber, esperoMesa); // Metodo para imprimir en forma de tabla.
        sem_wait(&cleanTables);
        tableRow(columnNumber, obtuveMesa);
        sem_post(&orderFood);
        tableRow(columnNumber, pidoComida);
        sem_wait(&deliveredFood);
        tableRow(columnNumber, obtuveComida);
        tableRow(columnNumber, comi);
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
    char tomePedido[] = "Tome un pedido";
    char saqueComida[] = "Saque una comida";
    char entreguePedido[] = "Entregue pedido";

    while (1) {
        sem_wait(&orderFood);
        tableRow(columnNumber, tomePedido); // Metodo para imprimir en forma de tabla.
        sem_wait(&foodQueueFull);
        tableRow(columnNumber, saqueComida);
        sem_post(&foodQueueEmpty);
        tableRow(columnNumber, entreguePedido);
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
    char inactivo[] = "Inactivo";
    char limpiando[] = "Limpiando mesa";
    char termineLimpiar[] = "Ya limpie";

    while (1) {
        tableRow(columnNumber, inactivo); // Metodo para imprimir en forma de tabla.
        sem_wait(&dirtyTables);
        tableRow(columnNumber, limpiando);
        tableRow(columnNumber, termineLimpiar);
        sem_post(&cleanTables);
    }
    pthread_exit(NULL);
}
