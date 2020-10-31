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

sem_t heladera_llena;
sem_t mutex_comprando;
sem_t mutex_reponiendo;

void* companiero(void* args);

int main() {
	pthread_t companieros[CANTIDAD_COMPANIEROS];
	sem_init(&heladera_llena,0,CANTIDAD_BOTELLAS);
	sem_init(&mutex_comprando,0,1);
	sem_init(&mutex_reponiendo,0,1);
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
		if (sem_trywait(&mutex_reponiendo) == 0) {
			sem_post(&mutex_reponiendo);
			if (sem_trywait(&heladera_llena) == 0) {
				printf("\e[%dmSoy el compañero %d\e[0m, tomando una botella de leche\n", 94+num, num);
				sleep(1);
			} else {
				printf("\e[%dmSoy el compañero %d\e[0m, no hay mas leche, ¿ya fue alguien a comprar?\n", 94+num, num);
				if (sem_trywait(&mutex_comprando) == 0) {
					printf("\e[%dmSoy el compañero %d\e[0m, aún no fue nadie, yendo al supermercado\n", 94+num, num);
					printf("\e[%dmSoy el compañero %d\e[0m, comprando botellas\n", 94+num, num);
					sleep(2);
					sem_wait(&mutex_reponiendo);
					for (i=0; i<CANTIDAD_BOTELLAS; i++) {
						sem_post(&heladera_llena);
						sleep(1);
						printf("\e[%dmSoy el compañero %d\e[0m, reponiendo botella nº %d\n", 94+num, num, (i+1));
					}
					printf("\e[%dmSoy el compañero %d\e[0m, ya repuse la heladera\n", 94+num, num);
					sem_post(&mutex_reponiendo);
					sleep(2);
					sem_post(&mutex_comprando);
				} else {
					printf("\e[%dmSoy el compañero %d\e[0m, ya hay alguien que fue a comprar\n", 94+num, num);
					sleep(2);
				}
			}
		} else {
			printf("\e[%dmSoy el compañero %d\e[0m, estan reponiendo la heladera, me voy a hacer otras cosas\n", 94+num, num);
			sleep(3);
		}
	}
}
