#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#define NELEMS(a) (sizeof(a) / sizeof((a)[0]))

static int
run(const char ***progs, size_t count)
{
	int (*pipes)[2];
	pid_t *children;
	size_t i, j;
	pid_t cur;
	int r, status;

	if (!(children = malloc(sizeof(*children) * count))) {
		fputs("out of memory\n", stderr);
		goto end;
	}

	/* ----------------- */
	/* TODO: crear pipes */
	/* ----------------- */

	for (i = 0; i < count; i++) {
		if ((cur = fork()) == -1) {
			fprintf(stderr, "fork [%zu]", i);
			perror("");
			r = -1;
			goto end;
		}
		if (cur == 0) {

			/* ------------------------------------ */
			/* TODO: redireccionar los fd adecuados */
			/* ------------------------------------ */

			if (execvp(progs[i][0], progs[i]) == -1) {
				perror("execvp");
				return -1;
			}
			exit(1); /* no alcanzable */
		} else {
			children[i] = cur;
		}
	}
	for (i = 0; i < count; i++) {
		if (waitpid(children[i], &status, 0) == -1) {
			perror("waitpid");
			return -1;
		}
		if (!WIFEXITED(status)) {
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
			    (int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;

end:
	free(children);
	free(pipes);

	return r;
}

int
main(int argc, char **argv)
{
	char *lscmd[] = { "ls", "-al", NULL };
	char *wccmd[] = { "wc", NULL };
	char *awkcmd[] = { "awk", "{ print $2 }", NULL };
	char **progs[] = { lscmd, wccmd, awkcmd };

	printf("status: %d\n", run(progs, NELEMS(progs)));
	fflush(stdout);
	fflush(stderr);

	return 0;
}
