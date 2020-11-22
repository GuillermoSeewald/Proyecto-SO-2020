#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "data.h"
#include "../Table/table.h"

typedef struct waiterMessage {
    char takingMeatOrderMessage[COLUMN_SIZE];
    char takingVegeOrderMessage[COLUMN_SIZE];
    char gettingMeatMenuMessage[COLUMN_SIZE];
    char gettingVegeMenuMessage[COLUMN_SIZE];
    char deliveringMeatOrderMessage[COLUMN_SIZE];
    char deliveringVegeOrderMessage[COLUMN_SIZE];
    char noMeatMenuMessage[COLUMN_SIZE];
    char noVegeMenuMessage[COLUMN_SIZE];
    char finish[COLUMN_SIZE];
} *waiterMsg;


waiterMsg getWaiterMessages();
void waiterWork();
int finish(int queueId);

int main(int argc, char **argv) {
    if (argc == 1) {
        waiterWork();
    } else {
        printf("Error de argumentos\n");
        exit(-1);
    }

    return 0;
}

/*
 * ALGORITMO CAMARERO:
    - repetir:
        - si hay un pedido de menu carne
            - si hay preparado un menu carne
                - notifico que saque un menu carne
                - entrego el menu de carne al cliente
            - sino
                - vuelvo a dejar el pedido de menu carne como pendiente
        - si hay un pedido de menu vegetariano
            - si hay preparado un menu vegetariano
                - notifico que saque un menu vegetariano
                - entrego el menu vegetariano al cliente
            - sino
                - vuelvo a dejar el pedido de menu vegatariano como pendiente
 */
void waiterWork() {
    waiterMsg printMsgs = getWaiterMessages();
    int columnNumber = 2;
    msg msg;
    int queueId = getQueue(getKey());

    while (!finish(queueId)) {
        if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_ORDER_MEAT_MENU, IPC_NOWAIT) != -1) {
            tableRow(columnNumber, printMsgs->takingMeatOrderMessage);
            if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_MEAT_QUEUE_FULL, IPC_NOWAIT) != -1) {
                tableRow(columnNumber, printMsgs->gettingMeatMenuMessage);
                msg.id = TYPE_MEAT_QUEUE_EMPTY;
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
                tableRow(columnNumber, printMsgs->deliveringMeatOrderMessage);
                msg.id = TYPE_DELIVERED_MEAT_MENU;
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            } else {
                tableRow(columnNumber, printMsgs->noMeatMenuMessage);
                msg.id = TYPE_ORDER_MEAT_MENU;
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            }
        }

        if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_ORDER_VEG_MENU, IPC_NOWAIT) != -1) {
            tableRow(columnNumber, printMsgs->takingVegeOrderMessage);
            if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_VEG_QUEUE_FULL, IPC_NOWAIT) != -1) {
                tableRow(columnNumber, printMsgs->gettingVegeMenuMessage);
                msg.id = TYPE_VEG_QUEUE_EMPTY;
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
                tableRow(columnNumber, printMsgs->deliveringVegeOrderMessage);
                msg.id = TYPE_DELIVERED_VEG_MENU;
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            } else {
                tableRow(columnNumber, printMsgs->noVegeMenuMessage);
                msg.id = TYPE_ORDER_VEG_MENU;
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            }
        }
    }

    tableRow(columnNumber, printMsgs->finish);
    free(printMsgs);
}

int finish(int queueId) {
    msg msg;
    return msgrcv(queueId, &msg, SIZE_MSG, TYPE_FINISH_WAITER, IPC_NOWAIT) != -1;
}

waiterMsg getWaiterMessages() {
    waiterMsg msg = (struct waiterMessage*) malloc(sizeof(struct waiterMessage));
    sprintf(msg->takingMeatOrderMessage, "Tome un pedido carne");
    sprintf(msg->gettingMeatMenuMessage, "Obtuve menu carne");
    sprintf(msg->deliveringMeatOrderMessage, "Entregue pedido carne");
    sprintf(msg->noMeatMenuMessage, "Faltan menu carne");
    sprintf(msg->takingVegeOrderMessage, "Tome un pedido vege");
    sprintf(msg->gettingVegeMenuMessage, "Obtuve menu vege");
    sprintf(msg->deliveringVegeOrderMessage, "Entregue pedido vege");
    sprintf(msg->noMeatMenuMessage, "Faltan menu vege");
    sprintf(msg->finish, "Me voy");
    return msg;
}
