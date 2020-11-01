#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#define MISSING_ARGUMENTS 1

void checkArguments(int argc);
int isHelpArgument(char* arg);
void help();
void execute(char* dirName, char* mode);
int getMode(char* mode);

int main(int argc, char **argv) {
    int i;
    checkArguments(argc);
    if (isHelpArgument(argv[1])) {
        help();
    } else {
        execute(argv[1], argv[2]);
    }
    exit(0);
}

void checkArguments(int argc) {
    if (argc < 2 || argc > 3) {
        printf("Cantidad de parámetros incorrecta, utilice -help para más información\n");
        exit(1);
    }
}

int isHelpArgument(char* arg) {
    return strcmp(arg, "-help")==0;
}

void help() {
    printf("Modo de empleo:\n");
    printf("  mkdir directory [mode]  Crea un directorio, si es que aún no existe, con m\n");
    printf("                          El argumento opcional mode especifica el modo para el nuevo directorio\n");
    printf("                          Si el mode no es especificado, se utiliza por defecto los permisos 0777\n");
    printf("  mkdir -help             Muestra esta ayuda y finaliza\n");
}

void execute(char* dirName, char* mode) {
    int m =  getMode(mode);
    printf("Creando directorio <%s> con mode <%d>\n", dirName, m);
    if(mkdir(dirName, m)){
    	perror("Error al crear directorio");
        exit(2);
    } else
        printf("Directorio <%s> creado correctamente\n", dirName);
}

int getMode(char* mode) {
    char *extra;
    long conv = strtol("0777", &extra, 8);
    int m;
    if (mode!=NULL){
        errno = 0;
        conv = strtol(mode, &extra, 8);
        if (errno != 0 || *extra != '\0' || conv > INT_MAX) {
            perror("Valor para el parámetro mode no válido");
            exit(-1);
        }
    }
    printf("Conv: %lo\n", conv);
    m = conv;
    return m;
}
