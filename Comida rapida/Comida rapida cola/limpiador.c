#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "data.h"
#include "../Table/table.h"

typedef struct cleanerMessage {
    char waitingDirtyTableMessage[COLUMN_SIZE];
    char cleaningTableMessage[COLUMN_SIZE];
    char tableCleanedMessage[COLUMN_SIZE];
} cleanerMsg;

cleanerMsg messages;

void cleanerWork();
void getCleanerMessages();
key_t getKey();
int getQueue(key_t key);

int main(int argc, char **argv) {
    if (argc == 1) {
        cleanerWork();
    } else {
        printf("Error de argumentos\n");
        exit(-1);
    }

    return 0;
}

/*
 * ALGORITMO LIMPIADOR:
    - repetir:
        - espero a que haya una mesa sucia
        - limpio la mesa sucia detectaad
        - notifico que hay una mesa limpia
 */
void cleanerWork() {
    getCleanerMessages();
    int columnNumber = 3;
    msg msg;
    int queueId = getQueue(getKey());

    while (1) {
        tableRow(columnNumber, messages.waitingDirtyTableMessage);
        msgrcv(queueId, &msg, SIZE_MSG, TYPE_DIRTY_TABLES, 0);
        tableRow(columnNumber, messages.cleaningTableMessage);
        tableRow(columnNumber, messages.tableCleanedMessage);
        msg.id = TYPE_CLEAN_TABLES;
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    }
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




void getCleanerMessages() {
    sprintf(messages.waitingDirtyTableMessage, "Esperando una mesa sucia");
    sprintf(messages.cleaningTableMessage, "Limpiando una mesa");
    sprintf(messages.tableCleanedMessage, "Limpie una mesa");
}
