#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include "errors.h"

void checkArguments(int argc);
int isHelpArgument(char* arg);
void help();
void execute(char* filename, char* mode);
long getMode(char* mode);

int main(int argc, char **argv) {
    checkArguments(argc);
    if (isHelpArgument(argv[1])) {
        help();
    } else {
        execute(argv[1], argv[2]);
    }
    exit(0);
}

void checkArguments(int argc) {
    if (argc < 3) {
        printf("Argumentos faltantes, utilice -help para más información\n");
        exit(MISSING_ARGUMENTS);
    } else {
        if (argc > 3) {
            printf("Demasiados argumentos, utilice -help para más información\n");
            exit(MISSING_ARGUMENTS);
        }
    }
}

int isHelpArgument(char* arg) {
    return strcmp(arg, "-help")==0;
}

void help() {
    printf("Modo de empleo:\n");
    printf("  chmod filename mode     Modifica los permisos del archivo (filename) indicado con el valor administrado (mode).\n");
    printf("  chmod -help             Muestra esta ayuda y finaliza\n");
}

void execute(char* filename, char* mode) {
    long m =  getMode(mode);
    printf("Modificando el archivo <%s>, estableciendo permisos <%lo>\n", filename, m);

    if(chmod(filename, m)){
    	perror("Error al modificar los permisos");
        exit(2);
    } else
        printf("Permisos modificados correctamente para el archivo %s\n", filename);
}

long getMode(char* mode) {
    char *extra;
    long conv;
    if (mode!=NULL){
        errno = 0;
        conv = strtol(mode, &extra, 8);
        if (errno != 0 || *extra != '\0' || conv > INT_MAX) {
            printf("Valor para el parámetro mode no válido, debe ser un entero positivo\n");
            exit(INVALID_ARGUMENTS);
        }
    }
    return conv;
}
