#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "data.h"

key_t getKey();
int getQueue(key_t key);
void execute(int idQueue, int num);

int main(int argc, char **argv) {
    if (argc == 2) {
        printf("Process id: %d\n", getpid());
        int numeroCompaniero = atoi(argv[1]);
        key_t key = getKey();
        printf("Key: %d in process %d\n", key, getpid());
        int idQueue = getQueue(key);
        printf("Queue: %d in process %d\n", idQueue, getpid());
        execute(idQueue, numeroCompaniero);
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
    int idQueue = msgget(key, 0777 | IPC_CREAT);
    if(idQueue == -1){
        perror("Error en la creacion de la cola de mensajes");
        exit(-1);
    }
}

void execute(int idQueue, int num) {
/*
    msgrcv -> recibir mensajes
    1 -> id cola mensajes
    2 -> mensaje donde voy a guardar
    3 -> tamaño de ese mensajes
    4 -> tipo de mensaje a buscar == id de mensaje
        _si se envía un tipo igual a 0, saca el primer mensaje de la cola
        _si se envía un tipo mayor a 0, busca especificamente un mensaje con ese tipo
    5 -> 0 ó IPC_NOWAIT lo más común pero hay otros valores posibles
        _si es 0, hasta que no haya un mensaje del tipo solicitado no avanza en el
        código (como un sem_wait)
        _si es IPC_NOWAIT, pide un mensaje del tipo solicitado pero si no encuentra retorna
        enseguida y se puede seguir con el resto del código (como un sem_trywait)

    msgsnd -> enviar mensajes
    1 -> id cola
    2 -> mensaje a enviar
    3 -> tamaño mensaje
    4 -> 0 ó IPC_NOWAIT lo más común pero hay otros valores posibles
        _si es 0, envía el mensaje a la cola y se queda esperando a que alguien lea lo que escribió
        _si es IPC_NOWAIT, envía el mensaje y sigue haciendo otras cosas, no importa cuando va
        a ser leído lo que escribió*/

    msg msgRcv, msgTypeHeladeraLlena, msgTypeHeladeraCerrada, msgTypeNadieComprando;
    msgTypeHeladeraLlena.id = TYPE_HELADERA_LLENA;
    msgTypeHeladeraCerrada.id = TYPE_HELADERA_CERRADA;
    msgTypeNadieComprando.id = TYPE_NADIE_COMPRANDO;
    int i;
    while(1) {
        // Chequeo si hay alguien reponiendo la heladera
        // Se busca en la cola un mensaje (message) de tipo TYPE_MUTEX_HELADERA_CERRADA
        // Si no hay ningún mensaje de ese tipo, significa que hay alguien reponiendo la heladera
        if (msgrcv(idQueue, &msgRcv, SIZE_MSG, TYPE_HELADERA_CERRADA, IPC_NOWAIT)!=-1) {

            // Ahora que abri la heladera, veo si hay botellas de leche
            if (msgrcv(idQueue, &msgRcv, SIZE_MSG, TYPE_HELADERA_LLENA, IPC_NOWAIT)!=-1) {
                printf("Soy el compañero %d, tomando una botella de leche\n", num);
                // Cierro la heladera
                msgsnd(idQueue, &msgTypeHeladeraCerrada, SIZE_MSG, IPC_NOWAIT);
                sleep(1);

            } else {
                // Cierro la heladera
                msgsnd(idQueue, &msgTypeHeladeraCerrada, SIZE_MSG, IPC_NOWAIT);

                // Como no había más botellas, veo si ya fue alguien al supermercado
                printf("Soy el compañero %d, no hay mas leche, ¿ya fue alguien a comprar?\n", num);

                // Control de si ya fue alguien, buscando un mensaje de tipo TYPE_MUTEX_COMPRANDO
                if (msgrcv(idQueue, &msgRcv, SIZE_MSG, TYPE_NADIE_COMPRANDO, IPC_NOWAIT)!=-1) {
                    // Si entro en este caso, significa que no fue nadie aún al supermercado
                    // El compañero va al supermercado
    				printf("Soy el compañero %d, aún no fue nadie, yendo al supermercado\n", num);
    				printf("Soy el compañero %d, comprando botellas\n", num);
                    sleep(2);

                    // Busco un mensaje de tipo TYPE_MUTEX_HELADERA_CERRADA para ver tener
                    // exclusividad al abrir la heladera
                    msgrcv(idQueue, &msgRcv, SIZE_MSG, TYPE_HELADERA_CERRADA, 0);


                    for (i=0; i<CANTIDAD_BOTELLAS; i++) {
                        // Una vez que se tiene exclusividad en la heladera, guarda todas
                        // las botellas compradas, enviando mensajes de tipo TYPE_HELADERA_LLENA
                        msgsnd(idQueue, &msgTypeHeladeraLlena, SIZE_MSG, IPC_NOWAIT);
                        sleep(1);
                        printf("Soy el compañero %d, reponiendo botella nº %d\n", num, (i+1));
                    }

                    // Al terminar de reponer, cierro la heladera
					printf("Soy el compañero %d, ya repuse la heladera\n", num);
                    msgsnd(idQueue, &msgTypeHeladeraCerrada, SIZE_MSG, IPC_NOWAIT);
					sleep(2);

                    // Indico que ya se volvió del supermercado con un mensaje
                    // de tipo TYPE_MUTEX_COMPRANDO
                    msgsnd(idQueue, &msgTypeNadieComprando, SIZE_MSG, IPC_NOWAIT);
                } else {
                    // Si ya fue alguien al supermercado, hago otras cosas
					printf("Soy el compañero %d, ya hay alguien que fue a comprar\n", num);
					sleep(2);
                }
            }
        } else {
            // Si hay alguien usando la heladera, hago otras cosas
            printf("Soy el compañero %d, la heladera está ocupada, me voy a hacer otras cosas\n", num);
            sleep(3);
        }
    }
}
