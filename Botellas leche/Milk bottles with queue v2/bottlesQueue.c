#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "data.h"

int getNumPartners();
key_t getKey();
int getQueue(key_t key);
void initQueue(int queueId);
void initPartners(int numPartners);
void setTerminationHandler();
void waitProcess(int numPartners);
void removeQueue(int queueId);

int main() {
    printf("-------------------------------------------------------------------------------------------\n");
    printf("Este programa genera una cantidad N de compañeros ingresado por el usuario para el problema\n");
    printf("\"Demasiadas botellas de leche\" utilizando cola de mensajes para la sincronización.\n");
    printf("-------------------------------------------------------------------------------------------\n");
    int numPartners = getNumPartners();
	key_t key = getKey();
    int queueId = getQueue(key);
    initQueue(queueId);
    initPartners(numPartners);
    setTerminationHandler();
    waitProcess(numPartners);
    removeQueue(queueId);

    return 0;
}

int getNumPartners() {
    int number;
    printf("Ingrese la cantidad de compañeros: ");
    scanf("%d", &number);
    return number;
}

key_t getKey() {
    //Obtengo la clave para cola de mensajes.
	key_t key = ftok(PATH,1);
	if(key == (key_t)-1){
		perror("Error al obtener la clave de la cola de mensajes");
		exit(-1);
	}
    return key;
}

int getQueue(key_t key) {
    //Se crea la cola de mensajes y se obtiene su id
    int queueId = msgget(key, 0777 | IPC_CREAT);
	if(queueId == -1){
		perror("Error en la creacion de la cola de mensajes");
		exit(-1);
	}
    return queueId;
}

void initQueue(int queueId) {
    msg msg;
    int i;

    // Agregando mensaje de heladera cerrada
    msg.id = TYPE_HELADERA;
    msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);

    // Agregando mensaje de nadie comprando
    msg.id = TYPE_MUTEX_COMPRAR;
    msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);

    // Agregando mensajes para la cantidad de botellas
    msg.id = TYPE_BOTELLAS;
    for (i=0; i<CANTIDAD_BOTELLAS; i++) {
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    }
}

void initPartners(int numPartners) {
    int i;
    for (i=0; i<numPartners; i++) {
        if (fork()==0) {
            char numeroCompaniero[30];
            sprintf(numeroCompaniero, "%d", (i+1));
            char* args[] = {"./partner", numeroCompaniero, NULL};
            execvp(args[0], args);
        }
    }
}

void waitProcess(int numPartners) {
    int i;
    for (i=0; i<numPartners; i++)
        wait(NULL);
}

void removeQueue(int queueId) {
    msgctl(queueId, IPC_RMID, (struct msqid_ds*) NULL);
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
