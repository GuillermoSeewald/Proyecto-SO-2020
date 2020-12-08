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
} custMsg;

custMsg messages;

void getCustomerMessages(int id);
void customerWork(int id);
key_t getKey();
int getQueue(key_t key);

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
    getCustomerMessages(id);
    int columnNumber = 0, menu;
    long menuType;
    msg msg;
    char* toPrint;
    int queueId = getQueue(getKey());

    while (1) {
        tableRow(columnNumber, messages.waitingTableMessage);
        msgrcv(queueId, &msg, SIZE_MSG, TYPE_CLEAN_TABLES, 0);
        menu = rand() % MAX_FOOD_TYPES;
        tableRow(columnNumber, messages.gettingTableMessage);
        if (menu) {
            msg.id = TYPE_ORDER_MEAT_MENU;
            toPrint = messages.orderingMeatMessage;
        }
        else {
            msg.id = TYPE_ORDER_VEG_MENU;
            toPrint = messages.orderingVegeMessage;
        }
        tableRow(columnNumber, toPrint);
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
        if (menu) {
            menuType = TYPE_DELIVERED_MEAT_MENU;
            toPrint = messages.gettingMeatMenuMessage;
        }
        else {
            menuType = TYPE_DELIVERED_VEG_MENU;
            toPrint = messages.gettingVegeMenuMessage;
        }
        msgrcv(queueId, &msg, SIZE_MSG, menuType ,0);
        tableRow(columnNumber, toPrint);
        tableRow(columnNumber, messages.leavingTableMessage);
        msg.id = TYPE_DIRTY_TABLES;
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
        sleep((1*id)%4);
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



void getCustomerMessages(int id) {
    sprintf(messages.waitingTableMessage, "%d espera mesa", id);
    sprintf(messages.gettingTableMessage, "%d obtuvo mesa", id);
    sprintf(messages.orderingMeatMessage, "%d pidio menu carne", id);
    sprintf(messages.gettingMeatMenuMessage, "%d obtuvo menu carne", id);
    sprintf(messages.orderingVegeMessage, "%d pidio menu vege", id);
    sprintf(messages.gettingVegeMenuMessage, "%d obtuvo menu vege", id);
    sprintf(messages.leavingTableMessage, "%d termino de comer", id);
}
