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
} chefMsg;

chefMsg messages;

void getChefMessages(int id);
void chefWork(int id);
key_t getKey();
int getQueue(key_t key);


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
        - espero a que haya espacio en alguna de las colas de comida
        - si la cola de comida que tiene espacio es la cola de comida de carne
            - preparo un menu de carne
        - sino (la cola de comida que tiene espacio es la cola de comida vegetariana)
            - preparo un menu vegetariano
 */
void chefWork(int id) {
    getChefMessages(id);
    int columnNumber = 1;
    msg msg;
    int queueId = getQueue(getKey());

    while (1) {
        msgrcv(queueId, &msg, SIZE_MSG, -2, 0);
        if (msg.id == TYPE_MEAT_QUEUE_EMPTY) {
            msg.id = TYPE_MEAT_QUEUE_FULL;
            tableRow(columnNumber, messages.cookingMeatMenuMessage);
            msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
        } else {
            msg.id = TYPE_VEG_QUEUE_FULL;
            tableRow(columnNumber, messages.cookingVegeMenuMessage);
            msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
        }
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




void getChefMessages(int id) {
    sprintf(messages.cookingMeatMenuMessage, "%d preparo menu carne", id);
    sprintf(messages.cookingVegeMenuMessage, "%d preparo menu vege", id);
}
