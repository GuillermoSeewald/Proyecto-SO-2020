#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MISSING_ARGUMENTS 1

void checkArguments(int argc);
int isHelpArgument(char* arg);
void help();
void execute(char* dirName);

int main(int argc, char **argv) {
    int i;
    checkArguments(argc);
    if (isHelpArgument(argv[1])) {
        help();
    } else {
        execute(argv[1]);
    }
    exit(0);
}

void checkArguments(int argc) {
    if (argc != 2) {
        printf("Cantidad de parámetros incorrecta, utilice -help para más información\n");
        exit(1);
    }
}

int isHelpArgument(char* arg) {
    return strcmp(arg, "-help")==0;
}

void help() {
    printf("Ayuda del mkdir\n");
}

void execute(char* dirName) {
    if(mkdir(dirName,0777)){
    	perror("Error al crear directorio");
        exit(2);
    }
}
