#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "data.h"
#include "../Table/table.h"
#define CUSTOMERS 50
#define CHEFS 3
#define TABLES 30
#define FOOD_MEAT_QUEUE_SIZE 10
#define FOOD_VEG_QUEUE_SIZE 10

void initQueue(int queueId);
void tableHeader();
void childProcess();
void finishProcess(int queueId);
void removeQueue(int queueId);

int main() {
    printf("--------------------------------------------------------------------------------\n");
    printf("\tEJERCICIO DE COMIDA RAPIDA CON COLA DE MENSAJES\n");
    printf("\n\tPresione <ENTER> para finalizar la ejecución\n");
    printf("--------------------------------------------------------------------------------\n");
    sleep(2);
    int queueId = getQueue(getKey());
    initQueue(queueId);
    tableHeader();
    childProcess();
    finishProcess(queueId);
    tableFooter();
    removeQueue(queueId);
    return 0;
}

void initQueue(int queueId) {
    int i;
    msg msg;
    msg.id = TYPE_MEAT_QUEUE_EMPTY;
    for (i=0; i<FOOD_MEAT_QUEUE_SIZE; i++)
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    msg.id = TYPE_VEG_QUEUE_EMPTY;
    for (i=0; i<FOOD_VEG_QUEUE_SIZE; i++)
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    msg.id = TYPE_CLEAN_TABLES;
    for (i=0; i<TABLES; i++)
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
}

void childProcess() {
    int i;
    pid_t pid;
    char id[5];
    char customerFilename[] = "./cliente", chefFilename[] = "./cocinero";
    char waiterFilename[] = "./camarero", cleanerFilename[] = "./limpiador";
    char* args[3];
    for (i=0; i<(CUSTOMERS+CHEFS+2);i++) {
        pid = fork();
        if (pid==0) {
            switch (i) {
                case 0 ... (CUSTOMERS-1): { // Se crea una cantidad de CUSTOMERS clientes
                    sprintf(id, "%d", (i+1));
                    args[0] = customerFilename;
                    args[1] = id;
                    args[2] = NULL;
                    break;
                }
                case CUSTOMERS ... (CUSTOMERS+CHEFS-1): { // Se crea una cantidad de CHEFS cocineros
                    sprintf(id, "%d", (i+1-CUSTOMERS));
                    args[0] = chefFilename;
                    args[1] = id;
                    args[2] = NULL;
                    break;
                }
                case CUSTOMERS+CHEFS: { // Se crea un camarero
                    args[0] = waiterFilename;
                    args[1] = NULL;
                    break;
                }
                case CUSTOMERS+CHEFS+1: { // Se crea un limpiador
                    args[0] = cleanerFilename;
                    args[1] = NULL;
                    break;
                }
            }
            execvp(args[0], args);
        } else
            if (pid < 0)
                perror("Error al crear proceso");
    }
}

void finishProcess(int queueId) {
    char finish;
    scanf("%c", &finish);
    int i;
    msg msg;

    // Finaliza los clientes
    msg.id = TYPE_FINISH_CUSTOMER;
    for (i=0; i<CUSTOMERS; i++) {
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
        wait(NULL);
    }

    // Finaliza el camarero
    msg.id = TYPE_FINISH_WAITER;
    msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    wait(NULL);

    // Finaliza el limpiador
    msg.id = TYPE_FINISH_CLEANER;
    msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    wait(NULL);

    // Finaliza los cocineros
    msg.id = TYPE_FINISH_CHEF;
    for (i=0; i<CHEFS; i++) {
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
        wait(NULL);
    }
}

void removeQueue(int queueId) {
    msgctl(queueId, IPC_RMID, (struct msqid_ds*) NULL);
}
