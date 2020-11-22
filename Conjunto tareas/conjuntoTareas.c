#include <pthread.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define TIME 1
#define SIZE_MSG_CANTIDAD sizeof(msgCantidad)

typedef struct messageTask {
    int type;
} *msgTask;

typedef struct messageCantidad {
    int cantidadTareas;
} msgCantidad;


int pipeA[2], pipeB[2], pipeC[2], pipeCoordinador[2];

void createPipes();
void createProcesses();
void coordinateTasks();
void finishProcesses();
void fourTasks();
void fiveTasks();
void sixTasks();
void taskCreator(void* taskWork(void*), int* mainPipe);
void* tareaA(void* args);
void* tareaB(void* args);
void* tareaC(void* args);
char* pickColor();

int main() {
    createPipes();
    createProcesses();

    close(pipeA[0]);
    close(pipeB[0]);
    close(pipeA[0]);
    close(pipeCoordinador[1]);
    coordinateTasks();
    close(pipeA[1]);
    close(pipeB[1]);
    close(pipeA[1]);
    close(pipeCoordinador[0]);
}

void createPipes() {
    if((pipe(pipeA) == -1) || (pipe(pipeB)==-1) || (pipe(pipeC)==-1) || (pipe(pipeCoordinador)==-1)){
        perror("Pipe");
        exit(-1);
    }
}

void createProcesses() {
    int i;
    for (i=0; i<3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            switch(i) {
                case(0):
                    close(pipeB[0]);
                    close(pipeB[1]);
                    close(pipeC[0]);
                    close(pipeC[1]);
                    taskCreator(tareaA, pipeA);
                    break;
                case(1):
                    close(pipeA[0]);
                    close(pipeA[1]);
                    close(pipeC[0]);
                    close(pipeC[1]);
                    taskCreator(tareaB, pipeB);
                    break;
                case(2):
                    close(pipeA[0]);
                    close(pipeA[1]);
                    close(pipeB[0]);
                    close(pipeB[1]);
                    taskCreator(tareaC, pipeC);
                    break;
            }
        } else
            if (pid==-1) {
                perror("Error al crear el proceso");
                exit(-1);
            }
    }
}

void coordinateTasks() {
    int read = 1;
    int invalidInput;
    while (read) {
        printf("Ingrese la cantidad de tareas a ejecutar, debe ser un entero entre 4 y 6: ");
        invalidInput = !scanf("%i", &read);
        while (invalidInput || read<4 || read>6) {
            printf("Opción no válida, ingrese nuevamente: ");
            invalidInput = scanf("%i", &read)==0;
            if (invalidInput)
                __fpurge(stdin);
        }

        switch (read) {
            case (4):
                fourTasks();
                break;
            case (5):
                fiveTasks();
                break;
            case (6):
                sixTasks();
                break;
        }

        printf("¿Desea seguir ejecutando? Ingrese <0> si desea terminar u otro caracter si desea continuar: ");
        scanf("%i", &read);
        __fpurge(stdin);
        printf("\n");
    }
    finishProcesses();
}

void finishProcesses() {
    msgCantidad msg;
    msg.cantidadTareas = -1;
    write(pipeA[1], &msg, SIZE_MSG_CANTIDAD);
    write(pipeB[1], &msg, SIZE_MSG_CANTIDAD);
    write(pipeC[1], &msg, SIZE_MSG_CANTIDAD);
}

void fourTasks() {
    msgCantidad msg;
    printf("\n-------------------------------------------------------------------\n");
    printf("\tEjecutando 4 tareas\n");
    msg.cantidadTareas = 2;
    write(pipeA[1], &msg, SIZE_MSG_CANTIDAD);
    write(pipeB[1], &msg, SIZE_MSG_CANTIDAD);
    read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
    read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
    printf("\tTerminaron las 4 tareas\n");
    printf("-------------------------------------------------------------------\n\n");
}

void fiveTasks() {
    msgCantidad msg;
    printf("\n-------------------------------------------------------------------\n");
    printf("\tEjecutando 5 tareas\n");
    msg.cantidadTareas = 2;
    write(pipeA[1], &msg, SIZE_MSG_CANTIDAD);
    write(pipeC[1], &msg, SIZE_MSG_CANTIDAD);
    msg.cantidadTareas = 1;
    write(pipeB[1], &msg, SIZE_MSG_CANTIDAD);
    read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
    read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
    read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
    printf("\tTerminaron las 5 tareas\n");
    printf("-------------------------------------------------------------------\n\n");
}

void sixTasks() {
    msgCantidad msg;
    printf("\n-------------------------------------------------------------------\n");
    printf("\tEjecutando 6 tareas\n");
    msg.cantidadTareas = 2;
    write(pipeA[1], &msg, SIZE_MSG_CANTIDAD);
    write(pipeB[1], &msg, SIZE_MSG_CANTIDAD);
    write(pipeC[1], &msg, SIZE_MSG_CANTIDAD);
    read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
    read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
    read(pipeCoordinador[0], &msg, SIZE_MSG_CANTIDAD);
    printf("\tTerminaron las 6 tareas\n");
    printf("-------------------------------------------------------------------\n\n");
}

void taskCreator(void* taskWork(void*), int* mainPipe) {
    // Se cierra solo el lado de escritura, el de lectura queda para activación en cada ciclo que se necesite
    close(mainPipe[1]);

    // Se cierra el lado de lectura del pipe coordinador, solo se necesita escribir
    // para indicar la finalizacion de las tareas.
    close(pipeCoordinador[0]);

    int i, cantidad = 1;
    msgCantidad msgCant;
    while(cantidad!=-1) {
        read(mainPipe[0], &msgCant, SIZE_MSG_CANTIDAD);
        cantidad = msgCant.cantidadTareas;
        if (cantidad!=-1) {
            pthread_t threads[cantidad];
            for(i=0; i<cantidad; i++) {
                msgTask msg = (struct messageTask*) malloc(sizeof(struct messageTask));
                msg->type = rand() % 2;
                pthread_create(&threads[i], NULL, taskWork, msg);
            }
            for (i=0; i< cantidad; i++) {
                pthread_join(threads[i], NULL);
            }
        }
        write(pipeCoordinador[1], "1", 2);
    }

    close(mainPipe[0]);
    close(pipeCoordinador[1]);
    exit(0);
}

void* tareaA(void* args) {
    msgTask msg = (struct messageTask*) args;
    int parcial = msg->type;
    char* color = pickColor();
    if (parcial) {
        printf("\t\tTAREA A: %s, el trabjo es parcial, requiere una unidad de tiempo\n", color);
        sleep(TIME);
    } else {
        printf("\t\tTAREA A: %s, el trabajo es total, requiere tres unidades de tiempo\n", color);
        sleep(TIME*3);
    }

    printf("\t\tTerminó una TAREA A\n");
    free(color);
    free(msg);
    pthread_exit(NULL);
}

void* tareaB(void* args) {
    msgTask msg = (struct messageTask*) args;
    int verificacion = msg->type;

    if (verificacion) {
        printf("\t\tTAREA B: Realizando verificación de frenos, requiere una unidad de tiempo\n");
        sleep(TIME);
    } else {
        printf("\t\tTAREA B: Realizando reparación de frenos, requiere dos unidades de tiempo\n");
        sleep(TIME*2);
    }

    printf("\t\tTerminó una TAREA B\n");
    free(msg);
    pthread_exit(NULL);
}

void* tareaC(void* args) {
    msgTask msg = (struct messageTask*) args;
    int reparacion = msg->type;

    if (reparacion) {
        int wheels = (rand() % 3) + 1;
        printf("\t\tTAREA C: Realizando trabajo de reparación de ruedas, hay %d rueda/s que necesita/n reparación, se requiere/n %d unidad/es de tiempo\n", wheels, wheels);
        sleep(TIME);
    }
    else {
        printf("\t\tTAREA C: Realizando trabajo de rotación y balanceo, se requieren tres unidades de tiempo\n");
        sleep(TIME*3);
    }

    printf("\t\tTerminó una TAREA C\n");
    free(msg);
    pthread_exit(NULL);
}

char* pickColor() {
    char format[] = "\e[%dmPintando de color %s\e[0m";
    char output[100];
    int color = rand() % 6;
    switch (color) {
        case 0:
            sprintf(output, format, (91+color), "rojo");
            break;
        case 1:
            sprintf(output, format, (91+color), "verde");
            break;
        case 2:
            sprintf(output, format, (91+color), "amarillo");
            break;
        case 3:
            sprintf(output, format, (91+color), "azul");
            break;
        case 4:
            sprintf(output, format, (91+color), "violeta");
            break;
        case 5:
            sprintf(output, format, (91+color), "celeste");
            break;
    }
    return strdup(output);
}
