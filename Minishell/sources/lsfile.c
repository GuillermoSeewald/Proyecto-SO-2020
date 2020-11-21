#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
    if (argc != 2) {
        printf("Cantidad de argumentos incorrecta, utilice -help para más información\n");
        exit(MISSING_ARGUMENTS);
    }
}

int isHelpArgument(char* arg) {
    return strcmp(arg, "-help")==0;
}

void help() {
    printf("Modo de empleo:\n");
    printf("  lsfile directory         Lista el contenido de la carpeta directory especificada\n");
    printf("  lsfile -help             Muestra esta ayuda y finaliza\n");
}

void execute(char* filename) {
    FILE *file = fopen(filename, "r");
    char* line = NULL;
    size_t len = 0;
    size_t read;

    if (file == NULL){
        printf("El archivo solicitado no existe dentro del directorio actual.\n");
    }
    else{
        while ((read = getline(&line, &len, file)) != (size_t) -1){
            printf("%s", line);
        }

        fclose(file);
        if (line)
            free(line);
    }
}
