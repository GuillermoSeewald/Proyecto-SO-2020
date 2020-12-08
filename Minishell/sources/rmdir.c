#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "errors.h"

void checkArguments(int argc);
int isHelpArgument(char* arg);
void help();
void execute(char* dirName);

int main(int argc, char **argv) {
    checkArguments(argc);
    if (isHelpArgument(argv[1])) {
        help();
    } else {
        execute(argv[1]);
    }
    exit(0);
}

void checkArguments(int argc) {
    if (argc < 2) {
        printf("Argumentos faltantes, utilice -help para más información\n");
        exit(MISSING_ARGUMENTS);
    } else {
        if (argc > 2) {
            printf("Cantidad de argumentos máxima excedida, utilice -help para más información\n");
            exit(EXCEEDED_ARGUMENTS);
        }
    }
}

int isHelpArgument(char* arg) {
    return strcmp(arg, "-help")==0;
}

void help() {
    printf("Modo de empleo:\n");
    printf("  rmdir directory     Elimina un directorio\n");
    printf("  rmdir -help         Muestra esta ayuda y finaliza\n");
}

void execute(char* dirName) {
    if(rmdir(dirName)){
    	perror("Error al eliminar el directorio");
        exit(2);
    }
}
