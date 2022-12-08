/*
 * Koriscenjem programskog jezika C kreirati Linux program koji kreira
 * dodatni proces sa kojim komunicira dvosmerno koriscenjem datavoda.
 * Roditeljski proces generise 10 slucajnih brojeva u opsegu od 200
 * do 299 i salje ih putem datavoda procesu detetu. Proces dete
 * roditeljskom procesu koriscenjem datavoda vraca samo brojeve
 * deljive sa 3 i to uvecane za 25. Roditeljski proces tako primljene 
 * brojeve od procesa deteta treba da odstampa na ekranu
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NRAND    10
#define LOW      200
#define HIGH     299
#define READ_FD  0
#define WRITE_FD 1

int main() {
	int child2parent[2] = {0}, parent2child[2] = {0};
	int broj = 0;
	srand(time(0));

	pipe(child2parent);
	pipe(parent2child);
	if(fork() == 0) {
		close(child2parent[READ_FD]);
		close(parent2child[WRITE_FD]);
		while(read(parent2child[READ_FD], &broj, sizeof(broj)) > 0) {
			if(broj % 3 == 0){
				broj += 25;
				write(child2parent[WRITE_FD], &broj, sizeof(broj));
			}
		}
		close(child2parent[WRITE_FD]);
		close(parent2child[READ_FD]);
		exit(0);
	}
	close(parent2child[READ_FD]);
	close(child2parent[WRITE_FD]);
	for(size_t i = 0; i < NRAND; i++) {
		broj = rand() % (HIGH - LOW) + LOW;
		write(parent2child[WRITE_FD], &broj, sizeof(broj));
	}
	close(parent2child[WRITE_FD]);
	while(read(child2parent[READ_FD], &broj, sizeof(broj)) > 0)
		printf("%d ", broj);
	printf("\n");
	close(child2parent[READ_FD]);
	return 0;
}
