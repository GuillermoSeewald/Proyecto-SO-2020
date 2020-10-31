#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio_ext.h>
#include <sys/wait.h>

#define SIZECOMMANDS 8
#define PATHSIZE 200
#define COMMAND 100
#define MAXARGUMENTS 10

char *commands[SIZECOMMANDS] = {"mkdir","rmdir","mkfile","lsdir","lsfile","perms","--help","exit"};

void welcomeMessage();
void printPath();
char* getCommand();
void splitArguments(char* command, char* splitedArguments[]);
int selectCommand(char* splitedArguments[]);
int findCommand(char* commandName);

void createDir(char* dir);
void removeDir(char* dir);
void createFile(char* file);
void lsDir(char* dir);
void lsFile(char *file);
void changeMod(char* path, char* mode);
void help();
void exec(char* exec[]);

int main(int argc, char **argv) {
	int exit = 0;
	welcomeMessage();
	do {
		printPath();
		char* args[MAXARGUMENTS];
		splitArguments(getCommand(), args);
		__fpurge(stdin); // Clean buffer
		exit = selectCommand(args);
	} while (!exit);
	return 0;
}

void welcomeMessage() {
	printf("------------------------------------------------------------------------------\n");
	printf("--------------------Minishell SO 2020 | Seewald - Giordano--------------------\n");
	printf("------------------------------------------------------------------------------\n\n");
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
 * Split a string into the arguments of the sent command and returns the number of arguments
 */
void splitArguments(char* command, char* args[]) {
	int i = 0;
	args[0] = strtok(command, " ");
	while (i < MAXARGUMENTS && args[i] != NULL) {
		i++;
		args[i] = strtok(NULL, " ");
	}
}

/*
 * Detects which command was selected and executes its corresponding set of instructions
 * with the necessary args
 */
int selectCommand(char* args[]) {
	int quit = 0;
	int i = findCommand(args[0]);
	if (i != 7) {
		if (!fork()) {
			switch(i) {
				case(0): {
					createDir(args[1]);
					break;
				}
				case(1): {
					removeDir(args[1]);
					break;
				}
				case(2): {
					createFile(args[1]);
					break;
				}
				case(3): {
					lsDir(args[1]);
					break;
				}
				case(4): {
					lsFile(args[1]);
					break;
				}
				case(5): {
					changeMod(args[1], args[2]);
					break;
				}
				case(6): {
					help();
					break;
				}
				default: {
					printf("Orden <<%s>> no encontrada, para información acerca de las ordenes disponibles consulte \"--help\"\n", args[0]);
					exit(0);
				}
			}
		}
	} else {
		quit = 1;
		printf("Gracias por utilizarme, vuelva prontos\n\n");
	}
	wait(NULL);
	return quit;
}

int findCommand(char* commandName) {
	int i = 0;
	while (i<SIZECOMMANDS && strcmp(commands[i], commandName)) {
		i++;
	}
	return i;
}

void createDir(char* dir){
	char* args[] = {"mkdir", dir, NULL};
	exec(args);
}

void removeDir(char* dir) {
	char* args[] = {"rmdir", dir, NULL};
	exec(args);
}


void createFile(char* file){
	char* args[] = {"touch", file, NULL};
	exec(args);
}


void lsDir(char* dir) {
	char* args[] = {"ls", dir, NULL};
	exec(args);
}

void lsFile(char *file){
	char* args[] = {"cat", file, NULL};
	exec(args);
}

void changeMod(char* path, char* mode) {
	char* args[] = {"chmod", path, mode, NULL};
	exec(args);
}

void help() {
	printf("Ayuda\n");
	exit(0);
}

void exec(char* args[]) {
	execvp(args[0], args);
}