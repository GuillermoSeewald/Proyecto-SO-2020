#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include "errors.h"

void checkArguments(int argc);
int isHelpArgument(char* arg);
void help();
void execute(char* dirName, char* mode);
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
    if (argc < 2) {
        printf("Argumentos faltantes, utilice -help para más información\n");
        exit(MISSING_ARGUMENTS);
    } else {
        if (argc > 3) {
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
    printf("  mkfile filename [mode]    Crea un archivo, si es que aún no existe\n");
    printf("                            El argumento opcional mode especifica los permisos para el nuevo directorio\n");
    printf("                            Si el mode no es especificado, se utiliza por defecto los permisos 0777\n");
    printf("  mkfile -help              Muestra esta ayuda y finaliza\n");
}

void execute(char* filename, char* mode) {
    long m =  getMode(mode);
    printf("Creando archivo <%s> con permisos <%lo>\n", filename, m);

    if(creat(filename, m)==-1){
    	perror("Error al crear archivo");
        exit(2);
    } else
        printf("Archivo <%s> creado correctamente\n", filename);
}

long getMode(char* mode) {
    char *extra;
    long conv = strtol("0777", &extra, 8);
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
