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
} waiterMsg;

waiterMsg messages;

void getWaiterMessages();
void waiterWork();
key_t getKey(int id);
int getQueue(key_t key);

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
        - espero a que haya un pedido
        - si el pedido es un menu de carne
            - espero a que haya un menu de carne preparado
            - obtengo el menu de carne
            - notifico que saque un menu de carne
            - entrego el menu de carne al cliente
        - sino, si el pedido es un menu vegetariano
            - espero a que haya un menu vegetariano preparado
            - obtengo el menu vegetariano
            - notifico que saque un menu vegetariano
            - entrego el menu vegetariano al cliente
 */
void waiterWork() {
    getWaiterMessages();
    int columnNumber = 2;
    msg msg;
    int mainQueueId = getQueue(getKey(KEY_ID_MAIN_QUEUE));
    int ordersQueueId = getQueue(getKey(KEY_ID_ORDERS_QUEUE));

    while (1) {
        msgrcv(ordersQueueId, &msg, SIZE_MSG, 0, IPC_CREAT);
        if (msg.id == TYPE_ORDER_MEAT_MENU) {
            tableRow(columnNumber, messages.takingMeatOrderMessage);
            msgrcv(mainQueueId, &msg, SIZE_MSG, TYPE_MEAT_QUEUE_FULL, 0);
            tableRow(columnNumber, messages.gettingMeatMenuMessage);
            msg.id = TYPE_MEAT_QUEUE_EMPTY;
            msgsnd(mainQueueId, &msg, SIZE_MSG, IPC_NOWAIT);
            tableRow(columnNumber, messages.deliveringMeatOrderMessage);
            msg.id = TYPE_DELIVERED_MEAT_MENU;
            msgsnd(mainQueueId, &msg, SIZE_MSG, IPC_NOWAIT);
        } else {
            tableRow(columnNumber, messages.takingVegeOrderMessage);
            msgrcv(mainQueueId, &msg, SIZE_MSG, TYPE_VEG_QUEUE_FULL, 0);
            tableRow(columnNumber, messages.gettingVegeMenuMessage);
            msg.id = TYPE_VEG_QUEUE_EMPTY;
            msgsnd(mainQueueId, &msg, SIZE_MSG, IPC_NOWAIT);
            tableRow(columnNumber, messages.deliveringVegeOrderMessage);
            msg.id = TYPE_DELIVERED_VEG_MENU;
            msgsnd(mainQueueId, &msg, SIZE_MSG, IPC_NOWAIT);
        }
    }
}

key_t getKey(int id) {
	key_t key = ftok(PATH, id);
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




void getWaiterMessages() {
    sprintf(messages.takingMeatOrderMessage, "Tome un pedido carne");
    sprintf(messages.gettingMeatMenuMessage, "Obtuve menu carne");
    sprintf(messages.deliveringMeatOrderMessage, "Entregue pedido carne");
    sprintf(messages.noMeatMenuMessage, "Faltan menu carne");
    sprintf(messages.takingVegeOrderMessage, "Tome un pedido vege");
    sprintf(messages.gettingVegeMenuMessage, "Obtuve menu vege");
    sprintf(messages.deliveringVegeOrderMessage, "Entregue pedido vege");
    sprintf(messages.noMeatMenuMessage, "Faltan menu vege");
}
