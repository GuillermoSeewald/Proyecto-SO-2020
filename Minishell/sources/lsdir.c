#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
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
        char defaultPath[] = ".";
        if (argc==1)
            execute(defaultPath);
        else
            execute(argv[1]);
    }

    exit(0);
}

void checkArguments(int argc) {
    if (argc > 2) {
        printf("Demasiados argumentos, utilice -help para más información\n");
        exit(EXCEEDED_ARGUMENTS);
    }
}

int isHelpArgument(char* arg) {
    return arg!=NULL && strcmp(arg, "-help")==0;
}

void help() {
    printf("Modo de empleo:\n");
    printf("  lsdir [directory]       Lista el contenido de la carpeta directory especificada\n");
    printf("                          Si el contenido no se especifica, se listará el directorio actual\n");
    printf("  lsdir -help             Muestra esta ayuda y finaliza\n");
}

void execute(char* path) {
    DIR* dir = opendir(path);
    struct dirent* entry;
    int i = 1;

    if (dir == NULL) {
        perror("No se pudo listar el directorio");
    }
    else {
        printf("Contenido del directorio:\n");
        while((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                printf("%5d.%3s%s\n", i, "", entry->d_name);
                i++;
            }
        }
    }
    closedir(dir);
}
