#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CMDEND "chau\n"

int
main(int argc, char **argv)
{
	char buf[1024];
	struct sockaddr_in local;
	ssize_t r;
	int sfd;

	/* crea socket */
	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	/* configura dirección */
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(5555);

	/* linkea socket con dirección */
	if (bind(sfd, (struct sockaddr *)&local, sizeof(local)) == -1) {
		perror("bind");
		exit(1);
	}

	/* atiende clientes */
	for (;;) {
		r = read(sfd, buf, sizeof(buf) - 1);
		if (r == -1) {
			perror("read");
			continue;
		}
		/* transforma en string de C */
		buf[r] = '\0';
		if (strncmp(buf, CMDEND, sizeof(CMDEND)) == 0)
			break;
		printf("< %s", buf);
	}

	/* cierra sockets */
	close(sfd);

	return 0;
}
