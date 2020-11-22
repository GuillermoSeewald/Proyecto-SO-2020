#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TIME 2
#define SIZE_MSG_CANTIDAD sizeof(msgCantidad)

typedef struct messageTareaA {
    int tipo;
    int color;
} *msgA;

typedef struct messageTareaBC {
    int tipo;
} *msgB, *msgC;

typedef struct messageCantidad {
    int cantidadTareas;
} msgCantidad;


int pipeA[2], pipeB[2], pipeC[2], pipeCoordinador[2];

void funcionA();
void funcionB();
void funcionC();
void* tareaA(void* args);
void* tareaB(void* args);
void* tareaC(void* args);




int main() {
    int i, leido;

    if((pipe(pipeA) == -1) || (pipe(pipeB)==-1) || (pipe(pipeC)==-1) || (pipe(pipeCoordinador)==-1)){
        perror("Pipe");
        exit(-1);
    }

    for (i=0; i<3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            switch(i) {
                case(0):
                    funcionA();
                    break;
                case(1):
                    funcionB();
                    break;
                case(2):
                    funcionC();
                    break;
            }
        } else
        if (pid==-1) {
            perror("Error al crear el proceso");
            exit(-1);
        }
    }


    msgCantidad msg;
    int end = 0;
    close(pipeA[0]);
    close(pipeB[0]);
    close(pipeA[0]);
    close(pipeCoordinador[1]);
    while (!end) {
        printf("Ingrese la cantidad de tareas a ejecutar, debe estar entre 4 y 6\n");
        scanf("%d\n", &leido);
        while (leido<4 || leido>6) {
            printf("Opción no válida, ingrese nuevamente\n");
            scanf("%d\n", &leido);
        }

        switch (leido) {
            case (4):
                printf("Ejecutando 4 tareas\n");
                msg.cantidadTareas = 2;
                write(pipeA[1], &msg, SIZE_MSG_CANTIDAD);
                write(pipeB[1], &msg, SIZE_MSG_CANTIDAD);
                read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
                read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
                printf("Terminaron las 4 tareas\n");
                break;
            case (5):
                printf("Ejecutando 5 tareas\n");
                msg.cantidadTareas = 2;
                write(pipeA[1], &msg, SIZE_MSG_CANTIDAD);
                write(pipeC[1], &msg, SIZE_MSG_CANTIDAD);
                msg.cantidadTareas = 1;
                write(pipeB[1], &msg, SIZE_MSG_CANTIDAD);
                read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
                read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
                read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
                printf("Terminaron las 5 tareas\n");
                break;
            case (6):
                printf("Ejecutando 6 tareas\n");
                msg.cantidadTareas = 2;
                write(pipeA[1], &msg, SIZE_MSG_CANTIDAD);
                write(pipeB[1], &msg, SIZE_MSG_CANTIDAD);
                write(pipeC[1], &msg, SIZE_MSG_CANTIDAD);
                read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
                read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
                read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
                printf("Terminaron las 6 tareas\n");
                break;
        }
        printf("¿Desea seguir ejecutando? 1->si, 0->no\n");
        scanf("%d\n", &end);
    }

    printf("TERMINA LA EJECUCION\n");
}

void funcionA() {
    close(pipeA[1]);
    close(pipeB[0]);
    close(pipeB[1]);
    close(pipeC[0]);
    close(pipeC[1]);
    close(pipeCoordinador[0]);
    int i, cantidad = 1;
    msgCantidad msgCant;

    while(cantidad!=-1) {
        read(pipeA[0], &msgCant, SIZE_MSG_CANTIDAD);
        cantidad = msgCant.cantidadTareas;
        if (cantidad!=-1) {
            pthread_t threads[cantidad];
            for(i=0; i< cantidad;i++ ){
                msgA msg = (struct messageTareaA*) malloc(sizeof(struct messageTareaA));
                msg->tipo = rand() % 2;
                msg->color = rand() % 5;
                pthread_create(&threads[i], NULL, tareaA, (void*) msg);
            }

            for (i=0; i< cantidad; i++) {
                pthread_join(threads[i], NULL);
            }
            printf("Terminaron las %d tareas de A\n", cantidad);
        }
        write(pipeCoordinador[1], "1", 2);
    }
    exit(0);
}

void funcionB() {
    close(pipeB[1]);
    close(pipeA[0]);
    close(pipeA[1]);
    close(pipeC[0]);
    close(pipeC[1]);
    close(pipeCoordinador[0]);
    int i, cantidad = 1;
    msgCantidad msgCant;

    while(cantidad!=-1) {
        read(pipeB[0], &msgCant, SIZE_MSG_CANTIDAD);
        cantidad = msgCant.cantidadTareas;
        if (cantidad!=-1) {
            pthread_t threads[cantidad];
            for(i=0; i< cantidad;i++ ){
                msgB msg = (struct messageTareaBC*) malloc(sizeof(struct messageTareaBC));
                msg->tipo = rand() % 2;
                pthread_create(&threads[i], NULL, tareaB, (void*) msg);
            }

            for (i=0; i< cantidad; i++) {
                pthread_join(threads[i], NULL);
            }
            printf("Terminaron las %d tareas de B\n", cantidad);
        }
        write(pipeCoordinador[1], "1", 2);
    }
    exit(0);
}

void funcionC() {
    close(pipeC[1]);
    close(pipeA[0]);
    close(pipeA[1]);
    close(pipeB[0]);
    close(pipeB[1]);
    close(pipeCoordinador[0]);
    int i;
    msgCantidad msgCant;
    int cantidad = 1;

    while(cantidad!=-1) {
        read(pipeC[0], &msgCant, SIZE_MSG_CANTIDAD);
        cantidad = msgCant.cantidadTareas;
        if (cantidad!=-1) {
            pthread_t threads[cantidad];
            for(i=0; i< cantidad;i++ ){
                msgC msg = (struct messageTareaBC*) malloc(sizeof(struct messageTareaBC));
                msg->tipo = rand() % 2;
                pthread_create(&threads[i], NULL, tareaC, (void*) msg);
            }

            for (i=0; i< cantidad; i++) {
                pthread_join(threads[i], NULL);
            }
            printf("Terminaron las %d tareas de C\n", cantidad);
        }
        write(pipeCoordinador[1], "1", 2);
    }
    exit(0);
}

void* tareaA(void* args) {
    msgA msg = (struct messageTareaA*) args;
    int parcial = msg->tipo;
    printf("Pintando de color %d, el tipo es %d\n", msg->color, parcial);
    if (parcial) {
        sleep(TIME);
    } else
        sleep(TIME*2);
    printf("Termino la tarea A\n");

    pthread_exit(NULL);
}

void* tareaB(void* args) {
    msgB msg = (struct messageTareaBC*) args;
    int verificacion = msg->tipo;

    printf("Realizando trabajo en los frenos, el tipo es %d\n", verificacion);
    if (verificacion)
        sleep(TIME);
    else
        sleep(TIME*2);

    pthread_exit(NULL);
}

void* tareaC(void* args) {
    msgC msg = (struct messageTareaBC*) args;
    int reparacion = msg->tipo;

    printf("Realizando trabajo en una rueda, el tipo es %d\n", reparacion);
    if (reparacion)
        sleep(TIME);
    else
        sleep(TIME*3);

    pthread_exit(NULL);
}