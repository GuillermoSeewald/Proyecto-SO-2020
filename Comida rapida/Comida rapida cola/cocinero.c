#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "data.h"
#include "../Table/table.h"

typedef struct chefMessage {
    char cookingMeatMenuMessage[COLUMN_SIZE];
    char cookingVegeMenuMessage[COLUMN_SIZE];
    char finish[COLUMN_SIZE];
} *chefMsg;

chefMsg getChefMessages(int id);
void chefWork(int id);
int finish(int queueId);

int main(int argc, char **argv) {
    if (argc == 2) {
        int id = atoi(argv[1]);
        chefWork(id);
    } else {
        printf("Error de argumentos\n");
        exit(-1);
    }

    return 0;
}

/*
 * ALGORITMO COCINERO:
    - repetir:
        - si hay espacio en la cola de comida de carne
            - preparo un menu de carne
        - si hay espacio en la cola de comida vegetariana
            - preparo un menu vegetariano
 */
void chefWork(int id) {
    chefMsg printMsgs = getChefMessages(id);
    int columnNumber = 1;
    msg msg;
    int queueId = getQueue(getKey());

    while (!finish(queueId)) {
        if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_MEAT_QUEUE_EMPTY, IPC_NOWAIT) != -1) {
            msg.id = TYPE_MEAT_QUEUE_FULL;
            msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            tableRow(columnNumber, printMsgs->cookingMeatMenuMessage);
        }
        if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_VEG_QUEUE_EMPTY, IPC_NOWAIT) != -1) {
            msg.id = TYPE_VEG_QUEUE_FULL;
            msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            tableRow(columnNumber, printMsgs->cookingVegeMenuMessage);
        }
    }

    tableRow(columnNumber, printMsgs->finish);
    free(printMsgs);
}

int finish(int queueId) {
    msg msg;
    return msgrcv(queueId, &msg, SIZE_MSG, TYPE_FINISH_CHEF, IPC_NOWAIT) != -1;
}

chefMsg getChefMessages(int id) {
    chefMsg msg = (struct chefMessage*) malloc(sizeof(struct chefMessage));
    sprintf(msg->cookingMeatMenuMessage, "%d preparo menu carne", id);
    sprintf(msg->cookingVegeMenuMessage, "%d preparo menu vege", id);
    sprintf(msg->finish, "%d se va", id);
    return msg;
}
