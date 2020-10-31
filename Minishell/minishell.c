#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/wait.h>
#define MAXARGUMENTS 6
#define PATHSIZE 100
#define INVALID_COMMAND 1

void printPath();
char* getCommand();
int splitArguments(char* command, char* args[]);
void execute(char** argv);
void changeCommandNameArg(char** argv);
int exitCommand(char* commandName);

int main() {
    int exit = 1;
    do {
        printPath();
        char* argv[MAXARGUMENTS];
        int argc = splitArguments(getCommand(), argv);
        __fpurge(stdin); // Clean buffer
        exit = exitCommand(argv[0]);
        if (exit)
            execute(argv);
    } while (exit);
    printf("Gracias por venir, vuelva prontos\n");
    return 0;
}

/*
 * Print working path
 */
void printPath() {
	char path[PATHSIZE];
	getcwd(path, PATHSIZE);
	printf("\e[93m%s $ \e[0m", path);
}

/*
 * Read a command by standard input
 */
char* getCommand() {
	char* newCommand = (char*) malloc(sizeof(char)*300);
	scanf("%[^\n]s", newCommand);
	return newCommand;
}

/*
 * Split a string into the arguments of the sent command
 */
int splitArguments(char* command, char* args[]) {
	int i = 0;
	args[0] = strtok(command, " ");
	while (i < MAXARGUMENTS && args[i] != NULL) {
		i++;
		args[i] = strtok(NULL, " ");
	}
    return i+1;
}

void execute(char** argv) {
    pid_t pid = fork();
    if (pid==0) {
        changeCommandNameArg(argv);
        execvp(argv[0], argv);
        perror("Invalid command");
        exit(INVALID_COMMAND);
    } else {
        if (pid>0)
            wait(NULL);
        else
            perror("No se pudo ejecutar el comando: \n");
    }
}

void changeCommandNameArg(char** argv) {
    char command[100] = "./sources/";
    strcat(command, argv[0]);
    argv[0] = command;
    printf("%s\n", argv[0]);
}

int exitCommand(char* commandName) {
    return strcmp(commandName, "exit");
}
