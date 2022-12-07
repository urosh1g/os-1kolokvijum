/*
 * Koriscenjem programskog jezika C napisati Linux program koji iz tekstualne datoteke ulaz.txt
 * ucitava niz celih bojeva (svaki u zasebnoj liniji). Glavni program odmah po startovanju
 * kreira dodatnu nit koja sortira brojeve ucitane u niz. Po zavrsetku sortiranja glavni program
 * stampa sortirani niz
 */

/*
 *!!!!!!!!!!!!!! ovaj kod ne ucitava iz ulaz.txt, vec trazi fajl kao parametar !!!!!!!!!!!!!!
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem_sort, sem_read;

int* array;
size_t nelem = 0, size = 10;
bool done = false;

void* sorting_thread(void* arg) {
	(void)arg;
	while(1) {
		if(done) break;
		sem_wait(&sem_sort);
		int idx = nelem - 1;
		while(idx >= 1 && array[idx] < array[idx - 1]){
			int tmp = array[idx];
			array[idx] = array[idx - 1];
			array[idx - 1] = tmp;
			idx--;
		}
		sem_post(&sem_read);
	}
	return NULL;
}

int main(int argc, char** argv) {
	pthread_t sorter;
	FILE* fd = NULL;

	if(argc < 2) {
		fprintf(stderr, "Usage: ./program <input file>\n");
		exit(EXIT_FAILURE);
	}
	if(!(fd = fopen(argv[1], "r"))) {
		fprintf(stderr, "ERROR: Cannot open file %s for reading!\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	array = (int*)malloc(size * sizeof(int));
	sem_init(&sem_read, 0, 1);
	sem_init(&sem_sort, 0, 0);
	pthread_create(&sorter, NULL, sorting_thread, &nelem);
	while(1) {
		sem_wait(&sem_read);
		if(fscanf(fd, "%d", &array[nelem++]) == EOF){
			done = true;
			sem_post(&sem_sort);
			break;
		}
		if(nelem >= size) {
			size *= 2;
			array = realloc(array, size);
		}
		sem_post(&sem_sort);
	}

	pthread_join(sorter, NULL);
	for(size_t i = 0; i < nelem; i++)
		printf("%d ", array[i]);
	printf("\n");
	sem_close(&sem_sort);
	sem_close(&sem_read);
	fclose(fd);
	free(array);
	return 0;
}
