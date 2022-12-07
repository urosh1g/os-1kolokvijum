/*
 * Koriscenjem programskog jezika C napisati Linux program koji kreira dodatni proces dete
 * Proces roditelj sa tastature ucitava putanju do tekstualne datoteke i jednu kljucnu rec.
 * Ova dva procesa komuniciraju putem cevi (pipe-a) od procesa roditela ka procesu detetu.
 * Proces roditelj koriscenjem pipe-a procesu detetu salje tu putanju do tekstualne datoteke i kljucnu rec.
 * Proces dete otvara tu teksutalnu datoteku i trazi u kojim linijama se javlja kljucna rec.
 * Proces dete na ekran stampa brojeve linija u kojima se javlja kljucna rec.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_FD  0
#define WRITE_FD 1

int main(int argc, char** argv) {
	int p[2] = {0};
	int buff_len, key_len;

	if(argc < 2) {
		fprintf(stderr, "Usage: ./program <input_file> <keyword>\n");
		exit(EXIT_FAILURE);
	}

	pipe(p);
	if(fork() == 0) {
		int nbytes = 0, status_code = EXIT_SUCCESS;
		char* buffer = NULL;
		char* keyword = NULL;
		char* line = NULL;
		FILE* fd = NULL;


		close(p[WRITE_FD]);
		read(p[READ_FD], &nbytes, sizeof(nbytes));
		buff_len = nbytes;
		buffer = (char*)malloc(nbytes * sizeof(char));
		read(p[READ_FD], buffer, nbytes);

		read(p[READ_FD], &nbytes, sizeof(nbytes));
		key_len = nbytes;
		keyword = (char*)malloc(nbytes * sizeof(char));
		read(p[READ_FD], keyword, nbytes);

		if(!(fd = fopen(buffer, "r"))) {
			fprintf(stderr, "ERROR: Cannot open file %s for reading!\n", buffer);
			goto cleanup;
		}
		ssize_t read = 0;
		size_t  line_len = 0, line_num = 1;
		while((read = getline(&line, &line_len, fd)) != -1) {
			if(strstr(line, keyword))
				printf("%lu, ", line_num);
			line_num++;	
		}
		printf("\n");

		fclose(fd);
cleanup:
		free(buffer);
		free(keyword);
		close(p[READ_FD]);
		exit(status_code);
	}
	close(p[READ_FD]);
	buff_len = strlen(argv[1]);
	key_len  = strlen(argv[2]);
	write(p[WRITE_FD], &buff_len, sizeof(buff_len));
	write(p[WRITE_FD], argv[1], buff_len);
	write(p[WRITE_FD], &key_len, sizeof(key_len));
	write(p[WRITE_FD], argv[2], key_len);
	wait(NULL);
	close(p[WRITE_FD]);
	return 0;
}
