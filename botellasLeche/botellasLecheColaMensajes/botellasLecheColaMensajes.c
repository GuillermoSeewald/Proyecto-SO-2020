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
void initQueue(int idQueue);
void initPartners(int numPartners);
void waitProcess(int numPartners);
void removeQueue(int idQueue);

int main() {
    int numPartners = getNumPartners();
	key_t key = getKey();
    int idQueue = getQueue(key);
    initQueue(idQueue);
    initPartners(numPartners);
    waitProcess(numPartners);
    removeQueue(idQueue);

    return 0;
}

int getNumPartners() {
    int number;
    printf("Ingrese la cantidad de compañeros\n");
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
    int idQueue = msgget(key, 0777 | IPC_CREAT);
	if(idQueue == -1){
		perror("Error en la creacion de la cola de mensajes");
		exit(-1);
	}
    return idQueue;
}

void initQueue(int idQueue) {
    msg msgTypeHeladeraLlena, msgTypeHeladeraCerrada, msgTypeNadieComprando;
    msgTypeHeladeraLlena.id = TYPE_HELADERA_LLENA;
    msgTypeHeladeraCerrada.id = TYPE_HELADERA_CERRADA;
    msgTypeNadieComprando.id = TYPE_NADIE_COMPRANDO;

    // Agregando mensaje de heladera cerrada
    msgsnd(idQueue, &msgTypeHeladeraCerrada, SIZE_MSG, IPC_NOWAIT);

    // Agregando mensaje de nadie comprando
    msgsnd(idQueue, &msgTypeNadieComprando, SIZE_MSG, IPC_NOWAIT);

    // Agregando mensajes para la cantidad de botellas
    int i;
    for (i=0; i<CANTIDAD_BOTELLAS; i++) {
        msgsnd(idQueue, &msgTypeHeladeraLlena, SIZE_MSG, IPC_NOWAIT);
    }
}

void initPartners(int numPartners) {
    int i;
    for (i=0; i<numPartners; i++) {
        if (fork()==0) {
            char numeroCompaniero[30];
            sprintf(numeroCompaniero, "%d", (i+1));
            char* args[] = {"./companiero", numeroCompaniero, NULL};
            execvp(args[0], args);
        }
    }
}

void waitProcess(int numPartners) {
    int i;
    for (i=0; i<numPartners; i)
        wait(NULL);
}

void removeQueue(int idQueue) {
    msgctl (idQueue, IPC_RMID, (struct msqid_ds *)NULL);
}
