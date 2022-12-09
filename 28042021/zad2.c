/*
 * Koriscenjem programskog jezika C kreirati Linux program koji
 * kopira datoteku cije se ime prosledjuje kao prvi argument
 * komandne linije tako sto kreira dodatni proces. Glavni proces
 * iz izvorisne datoteke cita N bajtova (N se programu prosledjuje
 * kao drugi argument komandne linije) i koriscenjem datavoda
 * procitane bajtove prosledjuje drugom procesu koji te bajtove
 * upisuje u odredisnu datoteku cije se ime prosledjuje kao treci argument
 * komandne linije. Ova procedura se ponavlja dok se ne iskopira
 * kompletna datoteka.
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define READ_FD  0
#define WRITE_FD 1

int main(int argc, char** argv) {
	int p[2] = {0};
	int n = 0;
	FILE* input_fd = NULL;

	if(argc < 3) {
		fprintf(stderr, "Usage: ./program <input_file> <Nbytes> <output_file>\n");
		exit(EXIT_FAILURE);
	}

	if(!(input_fd = fopen(argv[1], "r"))) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if((sscanf(argv[2], "%d", &n) != 1)) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	pipe(p);

	if(fork() == 0) {
		char* bytes = NULL;
		FILE* output_fd = NULL;

		if(!(output_fd = fopen(argv[3], "w"))) {
			fprintf(stderr, "%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		bytes = (char*)malloc(n * sizeof(char));

		close(p[WRITE_FD]);
		while(read(p[READ_FD], bytes, n) > 0)
			fputs(bytes, output_fd);
		close(p[READ_FD]);
		free(bytes);
		fclose(output_fd);
		exit(EXIT_SUCCESS);
	}
	close(p[READ_FD]);
	char* bytes = malloc(n * sizeof(char));
	int end = 0;
	while(1) {
		memset(bytes, 0, n);
		for(size_t i = 0; i < n; i++){
			bytes[i] = fgetc(input_fd);
			if(feof(input_fd)){
				end = 1;
				break;
			}
		}
		if(end) break;
		write(p[WRITE_FD], bytes, n);
	}
	free(bytes);
	fclose(input_fd);
	close(p[WRITE_FD]);
	return 0;
}
