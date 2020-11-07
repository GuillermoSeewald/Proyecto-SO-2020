#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#define EATING_TIME 2
#define CLEANING_TIME 1
#define CUSTOMERS 20
#define CHEFS 3
#define TABLES 10
#define FOOD_QUEUE_SIZE 10
#define COLUMN_SIZE 28
#define MAX_COLUMNS 4

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
void tab(int columnNumber, char* text);

int main() {
    pthread_t customers[CUSTOMERS], chefs[CHEFS], waiter, cleaner;
    int i;

    printf("          Cliente           |          Cocinero          |           Camarero         |          Limpiador         |\n");
    printf("--------------------------------------------------------------------------------------------------------------------\n");

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
    int columnNumber = 1;
    char* text = (char*) malloc(sizeof(char)*50);
    sprintf(text, "%d preparó comida", id);
    while (1) {
        sem_wait(&foodQueueEmpty);
        tab(columnNumber, text);
        sem_post(&foodQueueFull);
    }
    pthread_exit(NULL);
}

void* customerWork(void* args) {
    int id = *((int*) args);
    int columnNumber = 0;
    char* text1 = (char*) malloc(sizeof(char)*50);
    char* text2 = (char*) malloc(sizeof(char)*50);
    char* text3 = (char*) malloc(sizeof(char)*50);
    char* text4 = (char*) malloc(sizeof(char)*50);
    char* text5 = (char*) malloc(sizeof(char)*50);
    sprintf(text1, "%d espera mesa", id);
    sprintf(text2, "%d obtuvo mesa", id);
    sprintf(text3, "%d pidió comida", id);
    sprintf(text4, "%d obtuvo comida", id);
    sprintf(text5, "%d comió y se va", id);
    while (1) {
        tab(columnNumber, text1);
        sem_wait(&cleanTables);
        tab(columnNumber, text2);
        sem_post(&orderFood);
        tab(columnNumber, text3);
        sem_wait(&deliveredFood);
        tab(columnNumber, text4);
//        sleep(EATING_TIME);
        tab(columnNumber, text5);
        sem_post(&dirtyTables);
    }
    pthread_exit(NULL);
}

void* waiterWork(void* args) {
    int columnNumber = 2;
    char* text1 = (char*) malloc(sizeof(char)*50);
    char* text2 = (char*) malloc(sizeof(char)*50);
    char* text3 = (char*) malloc(sizeof(char)*50);
    sprintf(text1, "Tomé un pedido");
    sprintf(text2, "Obtuve comida de la cola");
    sprintf(text3, "Entregué un pedido");

    while (1) {
        sem_wait(&orderFood);
        tab(columnNumber, text1);
        sem_wait(&foodQueueFull);
        tab(columnNumber, text2);
        sem_post(&foodQueueEmpty);
        tab(columnNumber, text3);
        sem_post(&deliveredFood);
    }
    pthread_exit(NULL);
}

void* cleanerWork(void* args) {
    int columnNumber = 3;
    char* text1 = (char*) malloc(sizeof(char)*50);
    char* text2 = (char*) malloc(sizeof(char)*50);
    char* text3 = (char*) malloc(sizeof(char)*50);
    sprintf(text1, "Esperando mesa sucia");
    sprintf(text2, "Hay una mesa sucia");
    sprintf(text3, "Limpié una mesa");

    while (1) {
        tab(columnNumber, text1);
        sem_wait(&dirtyTables);
        tab(columnNumber, text2);
//        sleep(CLEANING_TIME);
        tab(columnNumber, text3);
        sem_post(&cleanTables);
    }
    pthread_exit(NULL);
}

char* emptyColumn(int start, int end) {
    char* emptyColumnsText = (char*) malloc(sizeof(char) * 170);
    char defaultText[] = "\e[93m-----\e[0m\0";
    int defaultTextSize = 5;
    int i, j, size1, size2;
    size1 = (COLUMN_SIZE - defaultTextSize)/2;
    size2 = size1 + ((COLUMN_SIZE - defaultTextSize)%2);

    for (i=start; i<end; i++) {
        for (j=0; j<size1; j++)
            strcat(emptyColumnsText, " ");
        strcat(emptyColumnsText, defaultText);
        for (j=0; j<size2; j++)
            strcat(emptyColumnsText, " ");
        strcat(emptyColumnsText, "|");
    }
    return emptyColumnsText;
}

void tab(int columnNumber, char* text) {
    char* tab = (char*) malloc(sizeof(char)*(200));
    int i, size1, size2;

    char* empty1 = emptyColumn(0, columnNumber);
    tab = strcat(tab, empty1);
    free(empty1);

    size1 = (COLUMN_SIZE - strlen(text))/2;
    size2 = size1 + ((COLUMN_SIZE - strlen(text))%2);
    for (i=0; i<size1; i++)
        strcat(tab, " ");
    strcat(tab, text);
    for (i=0; i<size2; i++)
        strcat(tab, " ");
    strcat(tab, "|");

    char* empty2 = emptyColumn(columnNumber+1, MAX_COLUMNS);
    strcat(tab, empty2);
    free(empty2);

    fflush(stdout);
    printf("%s\n", tab);
    free(tab);
//    usleep(400000);
//    sleep(1);
}
