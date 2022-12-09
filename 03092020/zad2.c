/*
 * Koriscenjem programskog jezika C napisati linux program koji omogucava
 * modifikaciju standardnog izlaza bilo kog programa i snimanje standardnog
 * izlaza u specificiranu datoteku. Naziv programa koji se startuje i ciji se
 * izlaz modifikuje se zadaje kao prvi argument komandne linije, a naziv 
 * tekstualne datoteke u koju se upisuje izlaz kao drugi argument komandne linije.
 * Standardni izlaz se modifikuje tako da se svako pojavljivanje karaktera 'b'
 * zamenjuje karakterom 'B'.
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define PARAM_LEN 16

#define READ_FD  0
#define WRITE_FD 1

int main(int argc, char** argv) {
	int p[2] = {0};
	char ch;
	FILE* fd = NULL;

	if(argc < 2) {
		fprintf(stderr, "Usage: ./program <prog> <output>\n");
		exit(EXIT_FAILURE);
	}

	if(!(fd = fopen(argv[2], "w"))) {
		fprintf(stderr, "ERROR: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	pipe(p);

	if(fork() == 0) {
		char* buffer[PARAM_LEN] = {0};
		close(p[READ_FD]);
		for(int i = 1; i < argc - 1; i++)
			buffer[i - 1] = argv[i];

		dup2(p[WRITE_FD], STDOUT_FILENO);
		if(execvp(buffer[0], buffer) == -1) {
			fprintf(stderr, "ERROR: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	wait(NULL);
	close(p[WRITE_FD]);
	while(read(p[READ_FD], &ch, sizeof(char)) > 0) {
		if(ch == 'b')
			ch = 'B';
		fputc(ch, fd);
	}
	close(p[READ_FD]);
	fclose(fd);
	return 0;
}
