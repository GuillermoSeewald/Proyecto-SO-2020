#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "errors.h"

void execute();

int main(int argc, char **argv) {
    execute();
    exit(0);
}

void execute() {
    printf("Comandos disponibles:\n");
    printf("    1. mkdir    Crea un directorio\n");
    printf("    2. rmdir    Eliminar un directorio\n");
    printf("    3. mkfile   Crea un archivo\n");
    printf("    4. lsdir    Lista el contenido de un directorio\n");
    printf("    5. lsfile   Muestra el contenido de un archivo\n");
    printf("    6. chmod    Modifica los permisos de un archivo\n");
    printf("    7. help     Muestra esta ayuda y finaliza\n");
    printf("    7. exit     Finaliza la ejecución de la minishell\n");
    printf("\nUtilice <nombre_de_comando> -help para obtener más información de un determinado comando\n");
}
