#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio_ext.h>

#define SIZECOMMANDS 8
#define PATHSIZE 200
#define COMMAND 100
#define MAXARGUMENTS 10

char *commands[SIZECOMMANDS] = {"mkdir","rmdir","mkfile","lsdir","lsfile","perms","help","quit"};

void printPath();
char* getCommand();
void splitArguments(char* command, char* splitedArguments[]);
int selectCommand(char* splitedArguments[]);

void createDir(char* dir);
void removeDir(char* dir);
void createFile(char* file);
void lsDir(char* dir);
void lsFile(char *file);
void changeMod(char* path, char* mode);
void help();

int main(int argc, char **argv) {
	int exit = 0;
	pid_t pid;
	if (argc == 1) {
		do {
			printPath();
			char* args[MAXARGUMENTS];
			splitArguments(getCommand(), args);
			__fpurge(stdin); // Clean buffer
			selectCommand(args);
			exit++;
			printf("Ejecucion numero %d\n", exit);
		} while (exit<3);
	}
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
	int exit = 0;
	lsFile(args[1]);
	/*
	int i = 0;
	while (i<SIZECOMMANDS && !strcmp(commands[i], splitedArguments[0])) {
		i++;
	}
	switch(i) {
		case(0): {
		// Crear directorio
			scanf("%s",dir)
			createDir(dir);
			break;
		}
		case(1): {
		//Eliminar directorio
		  scanf("%s",dir)
			removeDir(dir);
			break;
		}
		case(2): {
		//Crear archivo
			scanf("%s",file)
			createFile(file);
			break;
		}
		case(3): {
		//Listar Directorio
			scanf("%s",dir)
			lsDir(dir);
			break;
		}
		case(4): {
		//Listar Archivo
			scanf("%s",arc)
			lsFile(arc)
			break;
		}
		case(5): {
		//Cambiar permisos
			scanf("%s",path)
			scanf("%s",modo)
			changeMod(path,modo);
			break;
		}
		case(6): {
		//Mostrar ayuda del shell
			help();
			break;
		}
		case(7): {
			exit = 1
			//free malloc tendria q hacer
			break;
		}
		default {
			printf("Invalid command\n");
		}
	}*/
	return exit;
}

/**
*@brief Crea un directorio/s
*@param  dir direccion del directorio a crear
*/
void createDir(char* dir){
	 int resultCode;
	 resultCode = mkdir(dir,0777); //creo que asi queda con los permiso por defecto
	 if(resultCode != 0){
		 		perror("Error al crear directorio %s \n");
	 }
}

/**
*@brief Elimina directorio
*@param dir directorio a eliminar
*/
void removeDir(char* dir) {
	if (rmdir(dir) == -1) {
		perror("Remove dir");
	}
}

/**
*@brief Crea un archivo
*@param file archivo a crear
*/
void createFile(char* file){
	int resultCode;
	resultCode = creat(file,0777);//por ahora permisos por defecto
		if(resultCode == -1)
			perror("Error al crear archivo");
}


void lsDir(char* dir) {
	pid_t pid = fork();
	if (pid==0){
	char* args[] = {"ls", dir, NULL};
	execvp("ls", args);
exit(0);}
}

/**
*@brief Lista el contenido de un archivo pasado por parametro
*@param file el archivo a listar su contenido
*/
void lsFile(char *file){
	//tengo que hacer referencia al archivo con el nombre de file abrirlo y imprimir los caracteres con el fopen
		FILE *fptr;

		fptr=fopen(file, "r");//Abro el archivo
		if(fptr == NULL){
						printf("No se pudo abrir el archivo: %s\n", file);
		}else{
			//El archivo se procede a leer y imprimir el contenido del mismo
			char cont;
			cont=fgetc(fptr);
				while(cont != EOF){
								printf ("%c",cont);
								cont= fgetc(fptr);
			}
			fclose(fptr);
		}
}

/**
*@brief Modifica los permisos de un archivo
*@param path Direcion del archivo
*@param mode Permiso a cambiar
 */
void changeMod(char* path, char* mode) {
	int mod = atoi(mode);
	if (chmod(path, mod)) {
		perror("Change mod");
	}
}

void help() {
	printf("Ayuda\n");
}
