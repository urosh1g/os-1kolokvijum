/*
 * Koriscenjem programskog jezika C kreirati Linux program koji oponasa program more.
 * Ovaj program ocekuje dva argumenta komandne linije. Prvi argument je naziv programa
 * koji treba da se startuje i ciji izlaz da prikazuje ekran po ekran. Drugi argument
 * N je broj linija koje prikazuje na ekranu posle cega prikazuje poruku -- MORE --
 * ceka pritisak bilo kog tastera i nakon toga prikazuje narednih N linija itd..
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_FD  0
#define WRITE_FD 1

int main(int argc, char** argv) {
	char ch;
	size_t lines_to_show, lines = 0;
	int p[2] = {0};

	if(argc < 2) {
		fprintf(stderr, "Usage: ./program <input_program> <number_of_lines>\n");
		exit(EXIT_FAILURE);
	} 

	if(sscanf(argv[2], "%lu", &lines_to_show) != 1) {
		fprintf(stderr, "%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	pipe(p);

	if(fork() == 0) {
		close(p[READ_FD]);
		dup2(p[WRITE_FD], STDOUT_FILENO);
		execlp(argv[1], argv[1], NULL);
	}
	wait(NULL);
	close(p[WRITE_FD]);
	while(read(p[READ_FD], &ch, sizeof(ch)) > 0) {
		fputc(ch, stdout);
		if(ch == '\n')
			++lines;
		if(lines == lines_to_show) {
			lines = 0;
			fputs("--MORE--", stdout);
			fflush(stdout);
			getchar();
		}
	}
	close(p[READ_FD]);
	return 0;
}
