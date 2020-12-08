#define CANTIDAD_BOTELLAS 10
#define CANTIDAD_COMPANIEROS 2
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

struct arguments {
	int numeroCompaniero;
};
typedef struct arguments params;

sem_t heladera;
sem_t botellas;
sem_t mutex_comprar;
sem_t comprar;

void* companiero(void* args);

int main() {
	pthread_t companieros[CANTIDAD_COMPANIEROS];
	sem_init(&botellas,0,CANTIDAD_BOTELLAS);
	sem_init(&heladera,0,1);
	sem_init(&mutex_comprar,0,1);
	sem_init(&comprar,0,0);
	int i;
	for (i=0; i<CANTIDAD_COMPANIEROS; i++) {
		params* args = (params*) malloc(sizeof(params));
		args->numeroCompaniero = (i+1);
		pthread_create(&companieros[i], NULL, companiero, (void*) args);
	}

	for (i=0; i<CANTIDAD_COMPANIEROS; i++) {
		pthread_join(companieros[i], NULL);
	}
	return 0;
}

void* companiero(void* args) {
	params* arg = (params*) args;
	int i;
	int num = arg->numeroCompaniero;
	while(1) {
		sem_wait(&heladera);
		if (sem_trywait(&botellas)==0) {
			printf("\e[%dmSoy el compañero %d\e[0m, tomando una botella de leche\n", 94+num, num);
			if (sem_trywait(&botellas)==0)
				sem_post(&botellas);
			else {
				sem_wait(&mutex_comprar);
				printf("\e[%dmSoy el compañero %d\e[0m, se terminaron las botellas, aviso que hay que comprar\n", 94+num, num);
				sem_post(&comprar);
				sem_post(&mutex_comprar);
			}
		}
		sem_post(&heladera);

		sleep(1);

		sem_wait(&mutex_comprar);
		if (sem_trywait(&comprar)==0) {
			printf("\e[%dmSoy el compañero %d\e[0m, comprando botellas\n", 94+num, num);
			sem_wait(&heladera);
			for (i=0; i<CANTIDAD_BOTELLAS; i++) {
				printf("\e[%dmSoy el compañero %d\e[0m, reponiendo botella nº %d\n", 94+num, num, (i+1));
				sem_post(&botellas);
			}
			printf("\e[%dmSoy el compañero %d\e[0m, terminé de comprar botellas\n", 94+num, num);
			sem_post(&heladera);
		}
		sem_post(&mutex_comprar);
	}
}
