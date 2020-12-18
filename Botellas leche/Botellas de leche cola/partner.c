#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "data.h"

key_t getKey();
int getQueue(key_t key);
void execute(int queueId, int num);

int main(int argc, char **argv) {
    if (argc == 2) {
        int partnerId = atoi(argv[1]);
        key_t key = getKey();
        int queueId = getQueue(key);
        execute(queueId, partnerId);
    } else {
        printf("Error, cantidad de parámetros incorrecta\n");
        exit(-1);
    }

    exit(0);
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

void execute(int queueId, int partnerId) {
    msg msg;
    int i;
    while(1) {
        msgrcv(queueId, &msg, SIZE_MSG, TYPE_HELADERA, 0);
		if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_BOTELLAS, IPC_NOWAIT)!=-1) {
			printf("Soy el compañero %d, tomando una botella de leche\n", partnerId);
			if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_BOTELLAS, IPC_NOWAIT)!=-1) {
                msg.id = TYPE_BOTELLAS;
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            }
			else {
                msgrcv(queueId, &msg, SIZE_MSG, TYPE_MUTEX_COMPRAR, 0);
				printf("Soy el compañero %d, se terminaron las botellas, aviso que hay que comprar\n", partnerId);
                msg.id = TYPE_COMPRAR;
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
                msg.id = TYPE_MUTEX_COMPRAR;
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
			}
		}
        msg.id = TYPE_HELADERA;
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);


        msgrcv(queueId, &msg, SIZE_MSG, TYPE_MUTEX_COMPRAR, 0);
        if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_COMPRAR, IPC_NOWAIT)!=-1) {
			printf("Soy el compañero %d, comprando botellas\n", partnerId);
            msgrcv(queueId, &msg, SIZE_MSG, TYPE_HELADERA, 0);
            msg.id = TYPE_BOTELLAS;
			for (i=0; i<CANTIDAD_BOTELLAS; i++) {
				printf("Soy el compañero %d, reponiendo botella nº %d\n", partnerId, (i+1));
                msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
			}
			printf("Soy el compañero %d, terminé de comprar botellas\n", partnerId);
            msg.id = TYPE_HELADERA;
            msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
		}
        msg.id = TYPE_MUTEX_COMPRAR;
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    }
}
