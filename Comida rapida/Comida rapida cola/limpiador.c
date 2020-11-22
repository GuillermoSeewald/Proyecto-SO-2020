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
    char finish[COLUMN_SIZE];
} *cleanerMsg;

int finish(int queueId);
void cleanerWork();
cleanerMsg getCleanerMessages();

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
        - si hay una mesa sucia
            - limpio mesa mesa
            - nueva mesa limpia libre
 */
void cleanerWork() {
    cleanerMsg printMsgs = getCleanerMessages();
    int columnNumber = 3;
    msg msg;
    int queueId = getQueue(getKey());
    tableRow(columnNumber, printMsgs->waitingDirtyTableMessage);
    while (!finish(queueId)) {
        // Mira si hay mesa sucia. Se hace no bloqueante por una cuestión de que
        // pueda detectar el finish en la siguiente iteración en caso de que no haya
        // mas clientes (debido a que fueron terminados).
        if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_DIRTY_TABLES, IPC_NOWAIT)!=-1) {
            tableRow(columnNumber, printMsgs->cleaningTableMessage);
            tableRow(columnNumber, printMsgs->tableCleanedMessage);
            msg.id = TYPE_CLEAN_TABLES;
            msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            tableRow(columnNumber, printMsgs->waitingDirtyTableMessage);
        }
    }

    tableRow(columnNumber, printMsgs->finish);
    free(printMsgs);
}


int finish(int queueId) {
    msg msg;
    return msgrcv(queueId, &msg, SIZE_MSG, TYPE_FINISH_CLEANER, IPC_NOWAIT) != -1;
}


cleanerMsg getCleanerMessages() {
    cleanerMsg msg = (struct cleanerMessage*) malloc(sizeof(struct cleanerMessage));
    sprintf(msg->waitingDirtyTableMessage, "Esperando una mesa sucia");
    sprintf(msg->cleaningTableMessage, "Limpiando una mesa");
    sprintf(msg->tableCleanedMessage, "Limpie una mesa");
    sprintf(msg->finish, "Me voy");
    return msg;
}
