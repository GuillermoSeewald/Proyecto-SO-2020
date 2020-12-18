#include <pthread.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <string.h>
#define TIME 1
#define SIZE_MSG_TASK_VALUES sizeof(struct messageTaskData)

typedef struct messageTaskData {
    int type;
    int extraArg;
} msgTaskData;

typedef struct messageThread {
    struct messageTaskData** buffer;
    int* writePos;
    int* readPos;
    int bufferSize;
    sem_t* mutexBuffer;
    sem_t* endedTask;
    sem_t* fullBuffer;
} msgThread;

int pipeA[2], pipeB[2], pipeC[2], pipeCoordinador[2];

void createPipes();
void createProcesses();
void coordinateTasks();
void fourTasks();
void fiveTasks();
void sixTasks();
void finishProcesses();
void taskCreator(void* taskWork(void*), int* mainPipe);
void* tareaA(void* args);
void* tareaB(void* args);
void* tareaC(void* args);
char* pickColor(int color);

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

/*
 * Crea los diferentes pipes que van a ser utilizados para la comunicación entre
 * los diferentes procesos.
 */
void createPipes() {
    if((pipe(pipeA) == -1) || (pipe(pipeB)==-1) || (pipe(pipeC)==-1) || (pipe(pipeCoordinador)==-1)){
        perror("Pipe");
        exit(-1);
    }
}

/*
 * Crea los 3 procesos adicionales requeridos, los cuales manejaran la cantidad
 * de un tipo determinado de tareas.
 */
void createProcesses() {
    int i;
    for (i=0; i<3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            switch(i) {
                case(0):
                    close(pipeA[1]);
                    close(pipeB[0]);
                    close(pipeB[1]);
                    close(pipeC[0]);
                    close(pipeC[1]);
                    close(pipeCoordinador[0]);
                    taskCreator(tareaA, pipeA);
                    break;
                case(1):
                    close(pipeA[0]);
                    close(pipeA[1]);
                    close(pipeB[1]);
                    close(pipeC[0]);
                    close(pipeC[1]);
                    close(pipeCoordinador[0]);
                    taskCreator(tareaB, pipeB);
                    break;
                case(2):
                    close(pipeA[0]);
                    close(pipeA[1]);
                    close(pipeB[0]);
                    close(pipeB[1]);
                    close(pipeC[1]);
                    close(pipeCoordinador[0]);
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

/*
 * Lee y ejecuta la cantidad de tareas a ejecutar en el ciclo actual.
 */
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
    int size = -1;
    write(pipeA[1], &size, sizeof(int));
    write(pipeB[1], &size, sizeof(int));
    write(pipeC[1], &size, sizeof(int));
}

/*
 * Crea una cantidad igual a size de datos para tareas de tipo A y comunica dichos
 * datos al proceso A a través del pipeA
 * @param size: cantidad de datos a crear
 */
void createValuesTasksA(int size) {
    msgTaskData msg;
    int i;
    write(pipeA[1], &size, sizeof(int));
    for (i=0; i<size; i++) {
        msg.type = rand() % 2;
        msg.extraArg = rand() % 6;
        write(pipeA[1], &msg, SIZE_MSG_TASK_VALUES);
    }
}

/*
 * Crea una cantidad igual a size de datos para tareas de tipo B y comunica dichos
 * datos al proceso B a través del pipeB
 * @param size: cantidad de datos a crear
 */
void createValuesTasksB(int size) {
    msgTaskData msg;
    int i;
    write(pipeB[1], &size, sizeof(int));
    for (i=0; i<size; i++) {
        msg.type = rand() % 2;
        write(pipeB[1], &msg, SIZE_MSG_TASK_VALUES);
    }
}

/*
 * Crea una cantidad igual a size de datos para tareas de tipo C y comunica dichos
 * datos al proceso C a través del pipeC
 * @param size: cantidad de datos a crear
 */
void createValuesTasksC(int size) {
    msgTaskData msg;
    int i;
    write(pipeC[1], &size, sizeof(int));
    for (i=0; i<size; i++) {
        msg.type = rand() % 2;
        msg.extraArg = (rand() % 3) + 1;
        write(pipeC[1], &msg, SIZE_MSG_TASK_VALUES);
    }
}

/*
 * Se encarga de la sincronización cuando se tienen que ejecutar 4 tareas en el
 * ciclo actual. Se indica que se tienn que ejecutar dos tareas de tipo A y dos
 * tareas de tipo B y se espera a que se finalicen dichas tareas.
 */
void fourTasks() {
    int value;
    printf("\n-------------------------------------------------------------------\n");
    printf("\tEjecutando 4 tareas\n");

    createValuesTasksA(2);
    createValuesTasksB(2);

    read(pipeCoordinador[0], &value, sizeof(int));
    read(pipeCoordinador[0], &value, sizeof(int));
    printf("\tTerminaron las 4 tareas\n");
    printf("-------------------------------------------------------------------\n\n");
}

/*
 * Se encarga de la sincronización cuando se tienen que ejecutar 5 tareas en el
 * ciclo actual. Se indica que se tienn que ejecutar dos tareas de tipo A, una
 * tarea de tipo B y dos tareas de tipo C y luego se espera a que se finalicen
 * dichas tareas.
 */
void fiveTasks() {
    int value;
    printf("\n-------------------------------------------------------------------\n");
    printf("\tEjecutando 5 tareas\n");

    createValuesTasksA(2);
    createValuesTasksB(1);
    createValuesTasksC(2);

    read(pipeCoordinador[0], &value, sizeof(int));
    read(pipeCoordinador[0], &value, sizeof(int));
    read(pipeCoordinador[0], &value, sizeof(int));
    printf("\tTerminaron las 5 tareas\n");
    printf("-------------------------------------------------------------------\n\n");
}

/*
 * Se encarga de la sincronización cuando se tienen que ejecutar 5 tareas en el
 * ciclo actual. Se indica que se tienn que ejecutar dos tareas de tipo A, dos
 * tareas de tipo B y dos tareas de tipo C y luego se espera a que se finalicen
 * dichas tareas.
 */
void sixTasks() {
    int value;
    printf("\n-------------------------------------------------------------------\n");
    printf("\tEjecutando 6 tareas\n");

    createValuesTasksA(2);
    createValuesTasksB(2);
    createValuesTasksC(2);

    read(pipeCoordinador[0], &value, sizeof(int));
    read(pipeCoordinador[0], &value, sizeof(int));
    read(pipeCoordinador[0], &value, sizeof(int));
    printf("\tTerminaron las 6 tareas\n");
    printf("-------------------------------------------------------------------\n\n");
}

/*
 * Funcionalidad principal de los procesos hijos. Se encarga de la coordinación
 * de una x cantidad de tareas de un cierto tipo.
 * @param taskWork: conjunto de instrucciones que ejecutaran los hilos creados
 *                  por el proceso.
 * @param mainPipe: pipe mediante el cual el proceso recibirá los datos requeridos
 */
void taskCreator(void* taskWork(void*), int* mainPipe) {
    int i, cantidad = 0;
    msgThread msgThread; // Contendrá los datos requeridos por un hilo para su trabajo.
    int maxThreads = 2;
    pthread_t threads[maxThreads];
    sem_t mutexBuffer, fullBuffer, endedTask;

    // Buffer mediante el cual el proceso envía datos a los hilos
    msgThread.buffer = (struct messageTaskData**) malloc(sizeof(struct messageTaskData)*maxThreads);
    for (i=0; i<maxThreads; i++)
        msgThread.buffer[i] = (struct messageTaskData*) malloc(sizeof(struct messageTaskData));

    // Punteros de lectura y escritura en el buffer
    msgThread.writePos = (int*) malloc(sizeof(int));
    msgThread.readPos = (int*) malloc(sizeof(int));
    *(msgThread.writePos) = 0;
    *(msgThread.readPos) = 0;
    msgThread.bufferSize = maxThreads;

    // Semaforos con los cuales se realizará la sincronización entre el proceso y los hilos
    sem_init(&mutexBuffer, 0, 1); // Exclusividad de acceso al buffer.
    sem_init(&fullBuffer, 0, 0); // Notificación de que se cargó un dato en el buffer.
    sem_init(&endedTask, 0, 0); // Notificación por parte del hilo que terminó su trabajo.
    msgThread.mutexBuffer = &mutexBuffer;
    msgThread.fullBuffer = &fullBuffer;
    msgThread.endedTask = &endedTask;

    // Se inicializan los hilos
    for (i=0; i<maxThreads; i++)
        pthread_create(&threads[i], NULL, taskWork, (void*) &msgThread);

    // Una cantidad de -1 indica que el usuario indicó la finalización
    while(cantidad!=-1) {
        read(mainPipe[0], &cantidad, sizeof(int)); // Se lee la cantidad de tareas a ejecutar.
        if (cantidad!=-1) {
            for(i=0; i<cantidad; i++) {
                sem_wait(&mutexBuffer); // Exclusividad de acceso al buffer.
                read(mainPipe[0], msgThread.buffer[*(msgThread.writePos)], SIZE_MSG_TASK_VALUES); // Se lee un dato para una tarea.
                *(msgThread.writePos) = (*(msgThread.writePos) + 1) % msgThread.bufferSize; // Se modifica el puntero de escritura del buffer.
                sem_post(&fullBuffer); // Se notifica que se escribió un dato en el buffer.
                sem_post(&mutexBuffer); // Se libera la exclusividad del buffer.
            }
            for (i=0; i<cantidad; i++) {
                sem_wait(&endedTask); // Se espera a que finalice cada una de las tareas.
            }
        }
        write(pipeCoordinador[1], &cantidad, sizeof(int)); // Se notifica al proceso coordinador que se terminó.
    }

    for (i=0; i<maxThreads; i++)
        free(msgThread.buffer[i]);
    free(msgThread.buffer);
    free(msgThread.writePos);
    free(msgThread.readPos);

    close(mainPipe[0]);
    close(pipeCoordinador[1]);

    sem_destroy(&mutexBuffer);
    sem_destroy(&fullBuffer);
    sem_destroy(&endedTask);

    exit(0);
}

/*
 * Funcionalidad que debe seguir una tarea de tipo A.
 */
void* tareaA(void* args) {
    msgThread msg = *((struct messageThread*) args);
    int parcial, col;
    char* color;

    while (1) {
        sem_wait(msg.fullBuffer);
        sem_wait(msg.mutexBuffer);
        parcial = msg.buffer[*(msg.readPos)]->type;
        col = msg.buffer[*(msg.readPos)]->extraArg;
        *(msg.readPos) = (*(msg.readPos) + 1) % msg.bufferSize;
        sem_post(msg.mutexBuffer);

        color = pickColor(col);
        if (parcial) {
            printf("\t\tTAREA A: %s, el trabjo es parcial, requiere una unidad de tiempo\n", color);
            sleep(TIME);
        } else {
            printf("\t\tTAREA A: %s, el trabajo es total, requiere tres unidades de tiempo\n", color);
            sleep(TIME*3);
        }

        free(color);
        printf("\t\tTerminó una TAREA A\n");
        sem_post(msg.endedTask);
    }

    pthread_exit(NULL);
}

/*
 * Funcionalidad que debe seguir una tarea de tipo B.
 */
void* tareaB(void* args) {
    msgThread msg = *((struct messageThread*) args);
    int verificacion;

    while (1) {
        sem_wait(msg.fullBuffer);
        sem_wait(msg.mutexBuffer);
        verificacion = msg.buffer[*(msg.readPos)]->type;
        *(msg.readPos) = (*(msg.readPos) + 1) % msg.bufferSize;
        sem_post(msg.mutexBuffer);

        if (verificacion) {
            printf("\t\tTAREA B: Realizando verificación de frenos, requiere una unidad de tiempo\n");
            sleep(TIME);
        } else {
            printf("\t\tTAREA B: Realizando reparación de frenos, requiere dos unidades de tiempo\n");
            sleep(TIME*2);
        }

        printf("\t\tTerminó una TAREA B\n");
        sem_post(msg.endedTask);
    }

    pthread_exit(NULL);
}

/*
 * Funcionalidad que debe seguir una tarea de tipo C.
 */
void* tareaC(void* args) {
    msgThread msg = *((struct messageThread*) args);
    int reparacion, ruedas;

    while (1) {
        sem_wait(msg.fullBuffer);
        sem_wait(msg.mutexBuffer);
        reparacion = msg.buffer[*(msg.readPos)]->type;
        ruedas = msg.buffer[*(msg.readPos)]->extraArg;
        *(msg.readPos) = (*(msg.readPos) + 1) % msg.bufferSize;
        sem_post(msg.mutexBuffer);

        if (reparacion) {
            printf("\t\tTAREA C: Realizando trabajo de reparación de ruedas, hay %d rueda/s que necesita/n reparación, se requiere/n %d unidad/es de tiempo\n", ruedas, reparacion);
            sleep(TIME);
        }
        else {
            printf("\t\tTAREA C: Realizando trabajo de rotación y balanceo, se requieren tres unidades de tiempo\n");
            sleep(TIME*3);
        }

        printf("\t\tTerminó una TAREA C\n");
        sem_post(msg.endedTask);
    }

    pthread_exit(NULL);
}

char* pickColor(int color) {
    char format[] = "\e[%dmPintando de color %s\e[0m";
    char output[100];
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
