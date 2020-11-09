#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define CUSTOMERS 20
#define CHEFS 3
#define TABLES 10
#define FOOD_MEAT_QUEUE_SIZE 10
#define FOOD_VEG_QUEUE_SIZE 10

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

#define PATH "./data"
#define SIZE_MSG sizeof(struct message) - sizeof(long)

#define COLUMN_SIZE 26
#define MAX_COLUMNS 4

typedef struct message {
    long id;
} msg;

typedef struct customerMessage {
    char waitingTableMessage[COLUMN_SIZE];
    char gettingTableMessage[COLUMN_SIZE];
    char orderingMeatMessage[COLUMN_SIZE];
    char orderingVegeMessage[COLUMN_SIZE];
    char gettingMeatMenuMessage[COLUMN_SIZE];
    char gettingVegeMenuMessage[COLUMN_SIZE];
    char leavingTableMessage[COLUMN_SIZE];
} *custMsg;

typedef struct chefMessage {
    char cookingMeatMenuMessage[COLUMN_SIZE];
    char cookingVegeMenuMessage[COLUMN_SIZE];
} *chefMsg;

typedef struct waiterMessage {
    char takingMeatOrderMessage[COLUMN_SIZE];
    char takingVegeOrderMessage[COLUMN_SIZE];
    char gettingMeatMenuMessage[COLUMN_SIZE];
    char gettingVegeMenuMessage[COLUMN_SIZE];
    char deliveringMeatOrderMessage[COLUMN_SIZE];
    char deliveringVegeOrderMessage[COLUMN_SIZE];
    char noMeatMenuMessage[COLUMN_SIZE];
    char noVegeMenuMessage[COLUMN_SIZE];
} *waiterMsg;

typedef struct cleanerMessage {
    char waitingDirtyTableMessage[COLUMN_SIZE];
    char cleaningTableMessage[COLUMN_SIZE];
    char tableCleanedMessage[COLUMN_SIZE];
} *cleanerMsg;

char* defaultCenterText;
int queueId;

key_t getKey();
int getQueue(key_t key);
void initQueue();
void threads();
void* chefWork(void* args);
void* customerWork(void* args);
void* waiterWork(void* args);
void* cleanerWork(void* args);

chefMsg getChefMessages(int id);
custMsg getCustomerMessages(int id);
waiterMsg getWaiterMessages();
cleanerMsg getCleanerMessages();
void tableHeader();
char* centerColumn(char* text);
void tableRow(int columnNumber, char* text);

int main() {
    queueId = getQueue(getKey());
    initQueue();
    tableHeader();
    threads();
    return 0;
}

key_t getKey() {
	key_t key = ftok(PATH,1);
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

void initQueue() {
    int i;
    msg msg;
    msg.id = TYPE_MEAT_QUEUE_EMPTY;
    for (i=0; i<FOOD_MEAT_QUEUE_SIZE; i++)
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    msg.id = TYPE_VEG_QUEUE_EMPTY;
    for (i=0; i<FOOD_VEG_QUEUE_SIZE; i++)
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    msg.id = TYPE_CLEAN_TABLES;
    for (i=0; i<TABLES; i++)
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
}

/*
void removeQueue(int queueId) {
    msgctl(queueId, IPC_RMID, (struct msqid_ds*) NULL);
}*/

void threads() {
    pthread_t customers[CUSTOMERS], chefs[CHEFS], waiter, cleaner;
    int i;
    for (i=0; i<CUSTOMERS; i++) {
        pthread_create(&customers[i], NULL, customerWork, (void*) getCustomerMessages(i+1));
    }
    for (i=0; i<CHEFS; i++) {
        pthread_create(&chefs[i], NULL, chefWork, (void*) getChefMessages(i+1));
    }
    pthread_create(&waiter, NULL, waiterWork, (void*) getWaiterMessages());
    pthread_create(&cleaner, NULL, cleanerWork, (void*) getCleanerMessages());

    for (i=0; i<CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }
    for (i=0; i<CHEFS; i++) {
        pthread_join(chefs[i], NULL);
    }
    pthread_join(waiter, NULL);
    pthread_join(cleaner, NULL);
}

/*
 * ALGORITMO COCINERO:
    - repetir:
        - si hay espacio en la cola de comida de carne
            - preparo un menu de carne
        - si hay espacio en la cola de comida vegetariana
            - preparo un menu vegetariano
 */
void* chefWork(void* args) {
    chefMsg printMsgs = (struct chefMessage*) args;
    int columnNumber = 1, preparo = 0;
    msg msg;

    while (1) {
        if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_MEAT_QUEUE_EMPTY, IPC_NOWAIT) != -1) {
            msg.id = TYPE_MEAT_QUEUE_FULL;
            msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            tableRow(columnNumber, printMsgs->cookingMeatMenuMessage);
            preparo = 1;
        }
        if (msgrcv(queueId, &msg, SIZE_MSG, TYPE_VEG_QUEUE_EMPTY, IPC_NOWAIT) != -1) {
            msg.id = TYPE_VEG_QUEUE_FULL;
            msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
            tableRow(columnNumber, printMsgs->cookingVegeMenuMessage);
            preparo = 1;
        }

        // Duermo el hilo para que no esté constantemente chequeando en caso de que no
        // haya preparado ningún menú debido a que no habia espacio en las colas de comidas
        if (!preparo)
            sleep(2);
    }

    pthread_exit(NULL);
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
void* customerWork(void* args) {
    custMsg printMsgs = (struct customerMessage*) args;
    int columnNumber = 0, menu;
    long menuType;
    msg msg;
    char* toPrint;

    while (1) {
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
    pthread_exit(NULL);
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
void* waiterWork(void* args) {
    waiterMsg printMsgs = (struct waiterMessage*) args;
    int columnNumber = 2;
    msg msg;

    while (1) {
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

    pthread_exit(NULL);
}

/*
 * ALGORITMO LIMPIADOR:
    - repetir:
        - espero a que haya una mesa sucia
        - limpio mesa mesa
        - nueva mesa limpia libre
 */
void* cleanerWork(void* args) {
    cleanerMsg printMsgs = (struct cleanerMessage*) args;
    int columnNumber = 3;
    msg msg;
    while (1) {
        tableRow(columnNumber, printMsgs->waitingDirtyTableMessage);
        msgrcv(queueId, &msg, SIZE_MSG, TYPE_DIRTY_TABLES, 0);
        tableRow(columnNumber, printMsgs->cleaningTableMessage);
        tableRow(columnNumber, printMsgs->tableCleanedMessage);
        msg.id = TYPE_CLEAN_TABLES;
        msgsnd(queueId, &msg, SIZE_MSG, IPC_NOWAIT);
    }
    pthread_exit(NULL);
}





// --------------------------------------------------------------------
// Metodos para generar textos a imprimir
// --------------------------------------------------------------------

custMsg getCustomerMessages(int id) {
    custMsg msg = (struct customerMessage*) malloc(sizeof(struct customerMessage));
    sprintf(msg->waitingTableMessage, "%d espera mesa", id);
    sprintf(msg->gettingTableMessage, "%d obtuvo mesa", id);
    sprintf(msg->orderingMeatMessage, "%d pidio menu carne", id);
    sprintf(msg->gettingMeatMenuMessage, "%d obtuvo menu carne", id);
    sprintf(msg->orderingVegeMessage, "%d pidio menu vege", id);
    sprintf(msg->gettingVegeMenuMessage, "%d obtuvo menu vege", id);
    sprintf(msg->leavingTableMessage, "%d comio y se va", id);
    return msg;
}

chefMsg getChefMessages(int id) {
    chefMsg msg = (struct chefMessage*) malloc(sizeof(struct chefMessage));
    sprintf(msg->cookingMeatMenuMessage, "%d preparo menu carne", id);
    sprintf(msg->cookingVegeMenuMessage, "%d preparo menu vege", id);
    return msg;
}

waiterMsg getWaiterMessages() {
    waiterMsg msg = (struct waiterMessage*) malloc(sizeof(struct waiterMessage));
    sprintf(msg->takingMeatOrderMessage, "Tome un pedido carne");
    sprintf(msg->gettingMeatMenuMessage, "Obtuve menu carne");
    sprintf(msg->deliveringMeatOrderMessage, "Entregue pedido carne");
    sprintf(msg->noMeatMenuMessage, "No habia menu carne preparados");
    sprintf(msg->takingVegeOrderMessage, "Tome un pedido vege");
    sprintf(msg->gettingVegeMenuMessage, "Obtuve menu vege");
    sprintf(msg->deliveringVegeOrderMessage, "Entregue pedido vege");
    sprintf(msg->noMeatMenuMessage, "No habia menu vege preparados");
    return msg;
}

cleanerMsg getCleanerMessages() {
    cleanerMsg msg = (struct cleanerMessage*) malloc(sizeof(struct cleanerMessage));
    sprintf(msg->waitingDirtyTableMessage, "Esperando una mesa sucia");
    sprintf(msg->cleaningTableMessage, "Limpiando una mesa");
    sprintf(msg->tableCleanedMessage, "Limpie una mesa");
    return msg;
}

// --------------------------------------------------------------------
// Metódos para el formato de salida
// --------------------------------------------------------------------

void tableHeader() {
    char headerSep[COLUMN_SIZE];
    int i;
    for (i=0; i<COLUMN_SIZE; i++)
        headerSep[i] = '-';
    headerSep[COLUMN_SIZE] = '\0';

    char cl[] = "Cliente", co[] = "Cocinero", ca[] = "Camarero", li[] = "Limpiador";
    char* centerCl = centerColumn(cl); char* centerCo = centerColumn(co);
    char* centerCa = centerColumn(ca); char* centerLi = centerColumn(li);

    printf("|%s|%s|%s|%s|\n", headerSep, headerSep, headerSep, headerSep);
    printf("|%s|%s|%s|%s|\n", centerCl, centerCo, centerCa, centerLi);
    printf("|%s|%s|%s|%s|\n", headerSep, headerSep, headerSep, headerSep);
    free(centerCl); free(centerCo); free(centerCa); free(centerLi);

    char defaultText[] = "-----";
    defaultCenterText = centerColumn(defaultText);
}

char* centerColumn(char* text) {
    char column[COLUMN_SIZE];
    int sizeLeft, sizeRight;
    size_t sizeText;
    sizeText = strlen(text);
    sizeLeft = (COLUMN_SIZE - sizeText) / 2;
    sizeRight = sizeLeft + ((COLUMN_SIZE - sizeText) % 2);
    sprintf(column, "%*s%s%-*s", sizeLeft, "", text, sizeRight, "");
    return strdup(column);
}

void tableRow(int col, char* text) {
    char format[] = "|%s|%s|%s|%s|";
    char output[(COLUMN_SIZE+2)*MAX_COLUMNS];
    char* toFormat[4] = {
        defaultCenterText,
        defaultCenterText,
        defaultCenterText,
        defaultCenterText
    };
    toFormat[col] = centerColumn(text);
    sprintf(output, format, toFormat[0], toFormat[1], toFormat[2], toFormat[3]);
    printf("%s\n", output);
    free(toFormat[col]);
    sleep(2);
}
