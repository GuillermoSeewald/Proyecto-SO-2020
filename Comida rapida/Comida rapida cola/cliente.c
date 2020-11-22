#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "data.h"
#include "../Table/table.h"

typedef struct customerMessage {
    char waitingTableMessage[COLUMN_SIZE];
    char gettingTableMessage[COLUMN_SIZE];
    char orderingMeatMessage[COLUMN_SIZE];
    char orderingVegeMessage[COLUMN_SIZE];
    char gettingMeatMenuMessage[COLUMN_SIZE];
    char gettingVegeMenuMessage[COLUMN_SIZE];
    char leavingTableMessage[COLUMN_SIZE];
    char finish[COLUMN_SIZE];
} *custMsg;

custMsg getCustomerMessages(int id);
void customerWork(int id);
int finish(int queueId);

int main(int argc, char **argv) {
    if (argc == 2) {
        int id = atoi(argv[1]);
        customerWork(id);
    } else {
        printf("Error de argumentos\n");
        exit(-1);
    }

    return 0;
}

/*
 * ALGORITMO CLIENTE:
    - repetir:
        - espero mesa limpia disponible
        - ir a mesa
        - si pido menu carne
            - espero menu carne
        - sino, si pido menu vegetariano
            - espero menu vegetariano
        - como
        - libero la mesa (es decir, deja una mesa sucia)
 */
void customerWork(int id) {
    custMsg printMsgs = getCustomerMessages(id);
    int columnNumber = 0, menu;
    long menuType;
    msg msg;
    char* toPrint;
    int queueId = getQueue(getKey());

    while (!finish(queueId)) {
        tableRow(columnNumber, printMsgs->waitingTableMessage);
        msgrcv(queueId, &msg, SIZE_MSG, TYPE_CLEAN_TABLES, 0);
        menu = rand() % MAX_FOOD_TYPES;
        tableRow(columnNumber, printMsgs->gettingTableMessage);
        if (menu) {
            msg.id = TYPE_ORDER_MEAT_MENU;
            toPrint = printMsgs->orderingMeatMessage;
        }
        else {
            msg.id = TYPE_ORDER_VEG_MENU;
            toPrint = printMsgs->orderingVegeMessage;
        }
        tableRow(columnNumber, toPrint);
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
        if (menu) {
            menuType = TYPE_DELIVERED_MEAT_MENU;
            toPrint = printMsgs->gettingMeatMenuMessage;
        }
        else {
            menuType = TYPE_DELIVERED_VEG_MENU;
            toPrint = printMsgs->gettingVegeMenuMessage;
        }
        msgrcv(queueId, &msg, SIZE_MSG, menuType ,0);
        tableRow(columnNumber, toPrint);
        tableRow(columnNumber, printMsgs->leavingTableMessage);
        msg.id = TYPE_DIRTY_TABLES;
        msgsnd(queueId, &msg, SIZE_MSG , IPC_NOWAIT);
    }

    tableRow(columnNumber, printMsgs->finish);
    free(printMsgs);
}

int finish(int queueId) {
    msg msg;
    return msgrcv(queueId, &msg, SIZE_MSG, TYPE_FINISH_CUSTOMER, IPC_NOWAIT) != -1;
}


custMsg getCustomerMessages(int id) {
    custMsg msg = (struct customerMessage*) malloc(sizeof(struct customerMessage));
    sprintf(msg->waitingTableMessage, "%d espera mesa", id);
    sprintf(msg->gettingTableMessage, "%d obtuvo mesa", id);
    sprintf(msg->orderingMeatMessage, "%d pidio menu carne", id);
    sprintf(msg->gettingMeatMenuMessage, "%d obtuvo menu carne", id);
    sprintf(msg->orderingVegeMessage, "%d pidio menu vege", id);
    sprintf(msg->gettingVegeMenuMessage, "%d obtuvo menu vege", id);
    sprintf(msg->leavingTableMessage, "%d termino de comer", id);
    sprintf(msg->finish, "%d se va", id);
    return msg;
}
