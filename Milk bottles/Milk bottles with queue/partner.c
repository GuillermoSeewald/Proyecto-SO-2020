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
int finish(int queueId);
int isFridgeClosed(int queueId);
int isNotEmpty(int queueId);
void drinkBottleMessage(int partnerId);
void closeFridge(int queueId);
void isSomeoneAtStoreMessage(int partnerId);
int isNobodyAtStore(int queueId);
void atStoreMessage(int partnerId);
void waitClosedFridge(int queueId);
void restockAllBottles(int queueId, int partnerId);
void restockBottle(int queueId);
void restockBottlesMessage(int partnerId, int bottleNumber);
void restockFinishMessage(int partnerId);
void notifyBackFromStore(int queueId);
void someoneAlreadyAtStoreMessage(int partnerId);
void fridgeAlreadyOpenMessage(int partnerId);

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
}

void execute(int queueId, int partnerId) {
    /* ------------------------------------------------------------------- */
    /* ------------------------------------------------------------------- */
    /* ALGORITMO */

    /*
    iterar
        si la heladera no está ocupada
        |   ocupo la heladera
        |   si hay botellas
        |   |   saco una botella
        |   |   libero la heladera
        |   |   tomo la botella
        |   sino
        |       libero la heladera
        |       si nadie fue al supermercado
        |       |   voy al supermercado
        |       |   espero a que la heladera esté desocupada
        |       |   ocupo la heladera
        |       |   lleno la heladera con las botellas comprads
        |       |   libero la heladera
        |       |   notifico que ya volvi del supermercado
        |       sino
        |           espero haciendo otras cosas
        sino
            espero haciendo otras cosas

    */
    /* ------------------------------------------------------------------- */
    /* ------------------------------------------------------------------- */
    while(!finish(queueId)) {
        if (isFridgeClosed(queueId)) {
            if (isNotEmpty(queueId)) {
                closeFridge(queueId);
                drinkBottleMessage(partnerId);
            } else {
                closeFridge(queueId);
                isSomeoneAtStoreMessage(partnerId);

                if (isNobodyAtStore(queueId)) {
                    atStoreMessage(partnerId);
                    waitClosedFridge(queueId);
                    restockAllBottles(queueId, partnerId);
                    closeFridge(queueId);
                    notifyBackFromStore(queueId);
                } else {
					someoneAlreadyAtStoreMessage(partnerId);
                }
            }
        } else {
            fridgeAlreadyOpenMessage(partnerId);
        }
    }
}

int finish(int queueId) {
    msg msgRcv;
    return msgrcv(queueId, &msgRcv, SIZE_MSG, TYPE_FINISH, IPC_NOWAIT)!=-1;
}

int isFridgeClosed(int queueId) {
    msg msgRcv;
    return msgrcv(queueId, &msgRcv, SIZE_MSG, TYPE_CLOSED_FRIDGE, IPC_NOWAIT)!=-1;
}

int isNotEmpty(int queueId) {
    msg msgRcv;
    return msgrcv(queueId, &msgRcv, SIZE_MSG, TYPE_FULL_FRIDGE, IPC_NOWAIT)!=-1;
}

void drinkBottleMessage(int partnerId) {
    printf("Soy el compañero %d, tomando una botella de leche\n", partnerId);
    sleep(1);
}

void closeFridge(int queueId) {
    msg msg;
    msg.id = TYPE_CLOSED_FRIDGE;
    msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
}

void isSomeoneAtStoreMessage(int partnerId) {
    printf("Soy el compañero %d, no hay mas leche, ¿ya fue alguien a comprar?\n", partnerId);
}

int isNobodyAtStore(int queueId) {
    msg msgRcv;
    return msgrcv(queueId, &msgRcv, SIZE_MSG, TYPE_NOBODY_AT_STORE, IPC_NOWAIT)!=-1;
}

void atStoreMessage(int partnerId) {
    printf("Soy el compañero %d, aún no fue nadie, yendo al supermercado\n", partnerId);
    printf("Soy el compañero %d, comprando botellas\n", partnerId);
    sleep(2);
}

void waitClosedFridge(int queueId) {
    msg msgRcv;
    msgrcv(queueId, &msgRcv, SIZE_MSG, TYPE_CLOSED_FRIDGE, 0);
}

void restockAllBottles(int queueId, int partnerId) {
    int i;
    for (i=0; i<BOTTLES; i++) {
        restockBottle(queueId);
        restockBottlesMessage(partnerId, i+1);
    }
    restockFinishMessage(partnerId);
}

void restockBottle(int queueId) {
    msg msg;
    msg.id = TYPE_FULL_FRIDGE;
    msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
}

void restockBottlesMessage(int partnerId, int bottleNumber) {
    printf("Soy el compañero %d, reponiendo botella nº %d\n", partnerId, bottleNumber);
    sleep(1);
}

void restockFinishMessage(int partnerId) {
    printf("Soy el compañero %d, ya repuse la heladera\n", partnerId);
}

void notifyBackFromStore(int queueId) {
    msg msg;
    msg.id = TYPE_NOBODY_AT_STORE;
    msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
}

void someoneAlreadyAtStoreMessage(int partnerId) {
    printf("Soy el compañero %d, ya hay alguien que fue a comprar\n", partnerId);
    sleep(2);
}

void fridgeAlreadyOpenMessage(int partnerId) {
    printf("Soy el compañero %d, la heladera está ocupada, me voy a hacer otras cosas\n", partnerId);
    sleep(3);
}
