#ifndef DATA_H
#define DATA_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include "data.h"
#define MAX_FOOD_TYPES 2
#define TYPE_MEAT_QUEUE_EMPTY 3L
#define TYPE_MEAT_QUEUE_FULL 4L
#define TYPE_VEG_QUEUE_EMPTY 5L
#define TYPE_VEG_QUEUE_FULL 6L
#define TYPE_CLEAN_TABLES 7L
#define TYPE_DIRTY_TABLES 8L
#define TYPE_ORDER_MEAT_MENU 9L
#define TYPE_ORDER_VEG_MENU 10L
#define TYPE_DELIVERED_MEAT_MENU 11L
#define TYPE_DELIVERED_VEG_MENU 12L
#define TYPE_FINISH_CUSTOMER 13L
#define TYPE_FINISH_CHEF 14L
#define TYPE_FINISH_WAITER 15L
#define TYPE_FINISH_CLEANER 16L

typedef struct message {
    long id;
} msg;

#define PATH "./data.h"
#define SIZE_MSG sizeof(struct message) - sizeof(long)


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

#endif
