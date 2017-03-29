#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "config.h"

int
main(int argc, char **argv)
{
	char *buf;
	struct sockaddr_in name;
	size_t bufsiz;
	ssize_t w;
	int s;

	if (argc != 2) {
		fprintf(stderr, "uso: %s ip\n", argv[0]);
		exit(1);
	}

	/* crea el socket */
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	/* configura el nombre con el que se va a asociar el socket */
	memset(&name, 0, sizeof(name));
	if (!inet_aton(argv[1], &name.sin_addr)) {
		fprintf(stderr, "%s: ip desconocida\n", argv[1]);
		exit(1);
	}
	name.sin_family = AF_INET;
	name.sin_port = htons(PORT);

	/* conecta el socket con el servidor */
	if (connect(s, (struct sockaddr *)&name, sizeof(name)) == -1) {
		perror("connect");
		exit(1);
	}

	buf = NULL;
	bufsiz = 0;
	for (;;) {
		/* imprime un chirimbolo */
		printf("[%s]> ", argv[1]);

		/* lee una línea de la entrada estándar. Si no hay nada más
		 * para leer, sale */
		if ((w = getline(&buf, &bufsiz, stdin)) == -1) {
			if (!feof(stdin))
				perror("getline");
			break;
		}

		/* chequea si el comando es "chau" */
		if (strncmp(buf, ENDMSG, w) == 0)
			break;

		/* envía el comando al servidor */
		if (send(s, buf, w, 0) == -1) {
			perror("send");
			exit(1);
		}
	}

	/* libera el buffer y cierra el socket */
	free(buf);
	close(s);

	return 0;
}
