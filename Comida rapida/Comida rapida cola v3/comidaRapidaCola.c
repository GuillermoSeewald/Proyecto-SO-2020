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
void waitChildProcess();
void removeQueue(int queueId);
key_t getKey();
int getQueue(key_t key);
void setTerminationHandler();

int main() {
    printf("--------------------------------------------------------------------------------\n");
    printf("\tEJERCICIO DE COMIDA RAPIDA CON COLA DE MENSAJES\n");
    printf("--------------------------------------------------------------------------------\n");
    sleep(2);
    int queueId = getQueue(getKey());
    initQueue(queueId);
    tableHeader();
    childProcess();
    setTerminationHandler();
    waitChildProcess();
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

void waitChildProcess() {
    int i;
    for (i=0; i<CUSTOMERS; i++)
        wait(NULL);
    for (i=0; i<CHEFS; i++)
        wait(NULL);
    wait(NULL);
    wait(NULL);
}

void terminationHandler(int signum) {
    kill(0, SIGTERM);
}

void sigterm(int signum) {
}

void setTerminationHandler() {
    struct sigaction endAction, termAction;

    endAction.sa_handler = terminationHandler;
    sigemptyset(&endAction.sa_mask);
    endAction.sa_flags = 0;

    termAction.sa_handler = sigterm;
    sigemptyset(&termAction.sa_mask);
    termAction.sa_flags = 0;

    sigaction(SIGTERM, &termAction, NULL),
    sigaction(SIGINT, &endAction, NULL);
}

void removeQueue(int queueId) {
    msgctl(queueId, IPC_RMID, (struct msqid_ds*) NULL);
}

key_t getKey() {
	key_t key = ftok(PATH, 1);
	if(key == (key_t)-1){
		perror("Error al obtener la clave de la cola de mensajes");
		exit(-1);
	}
    return key;
}

int getQueue(key_t key) {
    int queueId = msgget(key, 0777 | IPC_CREAT);
	if(queueId == -1){
		perror("Error en la creacion de la cola de mensajes");
		exit(-1);
	}
    return queueId;
}
