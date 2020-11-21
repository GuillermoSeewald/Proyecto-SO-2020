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
void splitArguments(char* command, char* args[]);
void execute(char** argv);
void changeCommandNameArg(char** argv);
int exitCommand(char* commandName);

int main() {
    int exit = 0;
    char* command;
    do {
        printPath();
        char* argv[MAXARGUMENTS];
        command = getCommand();
        splitArguments(command, argv);
        __fpurge(stdin);
        exit = exitCommand(argv[0]);
        if (!exit)
            execute(argv);
        free(command);
        printf("\n");
    } while (!exit);
    printf("Gracias por venir, vuelva prontos\n");
    return 0;
}

/*
 * Print working path
 */
void printPath() {
	char path[PATHSIZE];
	getcwd(path, PATHSIZE);
	printf("\e[96m%s $ \e[0m", path);
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
void splitArguments(char* command, char* args[]) {
	int i = 0, j, quotationMark = 0, startArg = 0, argCount = 0;
    while (argCount < MAXARGUMENTS && command[i] != '\0') {
        if (command[i] == '\"') {
            quotationMark = !quotationMark;
            for (j=i; command[j] != '\0'; j++) {
                command[j] = command[j+1];
            }
            i--;
        }
        else
            if (command[i] == ' ' && !quotationMark) {
                command[i] = '\0';
                args[argCount] = &command[startArg];
                argCount++;
                startArg = i+1;
            }
        i++;
    }
    if (argCount<MAXARGUMENTS) {
        args[argCount] = &command[startArg];
        argCount++;
    }
    args[argCount] = NULL;
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
}

int exitCommand(char* commandName) {
    return strcmp(commandName, "exit")==0;
}
