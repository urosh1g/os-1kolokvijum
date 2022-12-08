/*
 *	Koriscenjem programskog jezika C napisati Linux program u kome kroisnik sa tastature
 *	unosi recenicu. Glavni program zatim kreira dve niti. Prva nit sortira reci u leksikografkski
 *	opadajucem redosledu, a po zavrsetku sortiranja druga nit na ekranu stampa tu recenicu
 *	tako da samo prva rec pocinje velikim slovom, a sve ostale reci malim. Po zavrsetku rada ove dve
 *	niti glavni program treba da odmastampa "KPAJ".
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CHARS 256

sem_t sem_sort, sem_print;
char recenica[MAX_CHARS] = {0};

char** izdvoji_reci(char* recenica);
void* sort_worker(void* arg);
void* print_worker(void* arg);

int main() {
	pthread_t sorter_thread, print_thread;
	sem_init(&sem_sort, 0, 1);
	sem_init(&sem_print, 0, 0);

	fgets(recenica, MAX_CHARS, stdin);
	char** reci = izdvoji_reci(recenica);

	pthread_create(&sorter_thread, NULL, sort_worker, (void*)reci);
	pthread_create(&print_thread, NULL, print_worker, (void*)reci);

	pthread_join(sorter_thread, NULL);
	pthread_join(print_thread, NULL);
	printf("KPAJ\n");

	free(reci);
	sem_destroy(&sem_sort);
	sem_destroy(&sem_print);
	return 0;
}

char** izdvoji_reci(char* recenica) {
	size_t size = 10;
	char* line = strtok(recenica, "\n");
	char** reci = (char**)malloc(size * sizeof(char*));

	for(size_t i = 0; i < size; i++) {
		reci[i] = NULL;
	}
	reci[0] = strtok(line, " ");
	size_t i = 1;
	while((reci[i++] = strtok(NULL, " "))) {
		if(i == size) {
			size *= 2;
			reci = realloc(reci, size * sizeof(char*));
			for(size_t j = size / 2; j < size; j++)
				reci[j] = NULL;
		}
	}
	return reci;
}

void* sort_worker(void* arg) {
	char** reci = (char**)arg;
	size_t len = 0;
	while(reci[len++]) {}
	--len;
	sem_wait(&sem_sort);
	for(size_t i = 0; i < len; i++) {
		int sorted = 1;
		for(size_t j = 0; j < len - i - 1; j++) {
			if(strcmp(reci[j], reci[j + 1]) < 0) {
				char* tmp = reci[j];
				reci[j] = reci[j + 1];
				reci[j + 1] = tmp;
				sorted = 0;
			}
		}
		if(sorted) break;
	} 
	sem_post(&sem_print);
	return NULL;
}

void* print_worker(void* arg) {
	char** reci = (char**)arg;
	size_t len = 0;
	while(reci[len++]) {}
	--len;
	sem_wait(&sem_print);
	if(reci[0][0] > 'A')
		reci[0][0] -= ('a' - 'A');
	printf("%s ", reci[0]);
	for(size_t i = 1; i < len; i++) {
		if(reci[i][0] < 'Z' && reci[i][0] > 'A')
			reci[i][0] -= ('a' - 'A');
		printf("%s ", reci[i]);
	}
	printf("\n");
	return NULL;
}
