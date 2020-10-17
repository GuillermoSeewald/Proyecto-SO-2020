#define SIZECOMMANDS 8
#define PATHSIZE 200
#define COMMAND 100
#define MAXARGUMENTS 20

char path[PATHSIZE];
char *commands[SIZECOMMANDS] = {"mkDir","rmDir","mkFile","lsDir","lsFile","perms","help","quit"};
char *actualCommand;
char *splitedArguments[MAXARGUMENTS];

int main(int argc, char **argv) {
	int exit = 0;

	if (argc == 1) {
		// Obtengo el directorio actual para mostrarlo en la consola
		getcwd(path, PATHSIZE);

		do {
			// Reservo memoria para obtener el comando deseado a ejecutar
			actualCommand = (char*) malloc(sizeof(char)*100);

			// Imprimo el directorio actual
			printf("\e[93m%s $ \e[0m", path);

			// Obtengo el comando ingresado
			scanf("%[^\n]s", actualCommand);

			// Separo los argumentos
			numberOfArguments = getArguments()

			__fpurge(stdin);

			selectCommand(numberOfArguments);
		} while (!exit)
	}
}

/*
 * Split a string into the arguments of the sent command
 */
int splitArguments() {
	int i = 0;
	splitedArguments[0] = strtok(actualCommand, " ");

	while (i < MAXARGUMENTS && splitedArguments[i] != NULL) {
		i++;
		splitedArguments[i] = strtol(NULL, " ");
	}

	return i;
}

void selectCommand(int numberOfArguments) {
	switch(splitedArguments[0]) {
		case(commands[0]): {
			break;
		}
		case(commands[1]): {
			break;
		}
		case(commands[2]): {
			break;
		}
		case(commands[3]): {
			break;
		}
		case(commands[4]): {
			break;
		}
		case(commands[5]): {
			break;
		}
		case(commands[6]): {
			break;
		}
		case(commands[7]): {
			break;
		}
		default {
			printf("Invalid arguments\n");
		}
	}
}