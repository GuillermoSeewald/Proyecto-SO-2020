#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#define EATING_TIME 2
#define CLEANING_TIME 1
#define CUSTOMERS 50
#define CHEFS 3
#define TABLES 30
#define FOOD_QUEUE_SIZE 10
#define COLUMN_SIZE 20
#define MAX_COLUMNS 4

sem_t foodQueueFull;
sem_t foodQueueEmpty;
sem_t cleantrles;
sem_t orderFood;
sem_t deliveredFood;
sem_t dirtytrles;
sem_t print;

char* defaultCenterText;

void threads();
void initSemaphores();
//void destroySemaphores();
void* chefWork(void* args);
void* customerWork(void* args);
void* waiterWork(void* args);
void* cleanerWork(void* args);
void tableHeader();
char* centerColumn(char* text);
void tr(int columnNumber, char* text);

int main() {
    initSemaphores();
    tableHeader();
    threads();

//    destroySemaphores();
    return 0;
}

void threads() {
    pthread_t customers[CUSTOMERS], chefs[CHEFS], waiter, cleaner;
    int i;
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
}

void initSemaphores() {
    sem_init(&foodQueueFull, 0, 0);
    sem_init(&foodQueueEmpty, 0, FOOD_QUEUE_SIZE);
    sem_init(&cleantrles, 0, TABLES);
    sem_init(&orderFood, 0, 0);
    sem_init(&deliveredFood, 0, 0);
    sem_init(&dirtytrles, 0, 0);
    sem_init(&print, 0, 1);
}

/*
void destroySemaphores() {
    sem_destroy(&foodQueueFull);
    sem_destroy(&foodQueueEmpty);
    sem_destroy(&cleantrles);
    sem_destroy(&orderFood);
    sem_destroy(&deliveredFood);
    sem_destroy(&dirtytrles);
}*/

void* chefWork(void* args) {
    int id = *((int*) args);
    int columnNumber = 1;
    char* text = (char*) malloc(sizeof(COLUMN_SIZE));
    sprintf(text, "%d preparo comida", id);
    while (1) {
        sem_wait(&foodQueueEmpty);
        tr(columnNumber, text);
        sem_post(&foodQueueFull);
    }
    pthread_exit(NULL);
}

void* customerWork(void* args) {
    int id = *((int*) args);
    int columnNumber = 0;
    char* text1 = (char*) malloc(sizeof(COLUMN_SIZE));
    char* text2 = (char*) malloc(sizeof(COLUMN_SIZE));
    char* text3 = (char*) malloc(sizeof(COLUMN_SIZE));
    char* text4 = (char*) malloc(sizeof(COLUMN_SIZE));
    char* text5 = (char*) malloc(sizeof(COLUMN_SIZE));
    sprintf(text1, "%d espera mesa", id);
    sprintf(text2, "%d obtuvo mesa", id);
    sprintf(text3, "%d pidio comida", id);
    sprintf(text4, "%d obtuvo comida", id);
    sprintf(text5, "%d comio y se va", id);
    while (1) {
        tr(columnNumber, text1);
        sem_wait(&cleantrles);
        tr(columnNumber, text2);
        sem_post(&orderFood);
        tr(columnNumber, text3);
        sem_wait(&deliveredFood);
        tr(columnNumber, text4);
        tr(columnNumber, text5);
        sem_post(&dirtytrles);
    }
    pthread_exit(NULL);
}

void* waiterWork(void* args) {
    int columnNumber = 2;
    char* text1 = (char*) malloc(sizeof(COLUMN_SIZE));
    char* text2 = (char*) malloc(sizeof(COLUMN_SIZE));
    char* text3 = (char*) malloc(sizeof(COLUMN_SIZE));
    sprintf(text1, "Tome un pedido");
    sprintf(text2, "Obtuve comida");
    sprintf(text3, "Entregue pedido");

    while (1) {
        sem_wait(&orderFood);
        tr(columnNumber, text1);
        sem_wait(&foodQueueFull);
        tr(columnNumber, text2);
        sem_post(&foodQueueEmpty);
        tr(columnNumber, text3);
        sem_post(&deliveredFood);
    }
    pthread_exit(NULL);
}

void* cleanerWork(void* args) {
    int columnNumber = 3;
    char* text1 = (char*) malloc(sizeof(COLUMN_SIZE));
    char* text2 = (char*) malloc(sizeof(COLUMN_SIZE));
    char* text3 = (char*) malloc(sizeof(COLUMN_SIZE));
    sprintf(text1, "Inactivo");
    sprintf(text2, "Limpiando mesa");
    sprintf(text3, "Ya limpie");

    while (1) {
        tr(columnNumber, text1);
        sem_wait(&dirtytrles);
        tr(columnNumber, text2);
        tr(columnNumber, text3);
        sem_post(&cleantrles);
    }
    pthread_exit(NULL);
}

void tableHeader() {
    char* headerSep = (char*) malloc(sizeof(COLUMN_SIZE));
    int i;
    for (i=0; i<COLUMN_SIZE; i++)
        headerSep[i] = '-';
    headerSep[COLUMN_SIZE] = '\0';
    char cl[] = "Cliente";
    char co[] = "Cocinero";
    char ca[] = "Camarero";
    char li[] = "Limpiador";
    printf("|%s|%s|%s|%s|\n", headerSep, headerSep, headerSep, headerSep);
    printf("|%s|%s|%s|%s|\n", centerColumn(cl), centerColumn(co), centerColumn(ca), centerColumn(li));
    printf("|%s|%s|%s|%s|\n", headerSep, headerSep, headerSep, headerSep);

    char defaultText[] = "-----";
    defaultCenterText = centerColumn(defaultText);
}

char* centerColumn(char* text) {
    char* column = (char*) malloc(sizeof(COLUMN_SIZE));
    int sizeLeft, sizeRight;
    size_t sizeText;
    sizeText = strlen(text);
    sizeLeft = (COLUMN_SIZE - sizeText) / 2;
    sizeRight = sizeLeft + ((COLUMN_SIZE - sizeText) % 2);
    sprintf(column, "%*s%s%-*s", sizeLeft, "", text, sizeRight, "");
    return column;
}

void tr(int col, char* text) {
    char format[] = "|%s|%s|%s|%s|";
    char output[(COLUMN_SIZE+2)*MAX_COLUMNS];
    char* toFormat[4] = {
        defaultCenterText,
        defaultCenterText,
        defaultCenterText,
        defaultCenterText
    };
    toFormat[col] = centerColumn(text);
    sprintf(output, format, toFormat[0], toFormat[1], toFormat[2], toFormat[3]);
    printf("%s\n", output);
//    sleep(1);
}
