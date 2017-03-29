#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKPATH "socket-e"

int
main(int argc, char **argv)
{
	char buf[1024];
	struct sockaddr_un local, remote;
	ssize_t r;
	int cfd, remotelen, sfd;

	/* crea socket */
	if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	/* configura dirección */
	memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
	strncpy(local.sun_path, SOCKPATH, sizeof(local.sun_path) - 1);
	unlink(SOCKPATH);

	/* linkea socket con dirección */
	if (bind(sfd, (struct sockaddr *)&local, sizeof(local)) == -1) {
		perror("bind");
		exit(1);
	}

	/* pone en modo "aceptar conexión" */
	if (listen(sfd, 10) == -1) {
		perror("listen");
		exit(1);
	}

	/* espera conexión */
	remotelen = sizeof(remote);
	if ((cfd = accept(sfd, (struct sockaddr *)&remote, (socklen_t *)&remotelen))
	    == -1) {
		perror("accept");
		exit(1);
	}

	/* atiende cliente */
	for (;;) {
		r = recv(cfd, buf, sizeof(buf) - 1, 0);
		if (r == -1) {
			perror("recv");
			continue;
		} else if (r == 0) {
			break;
		}
		/* transforma en string de C */
		buf[r] = '\0';
		printf("< %s", buf);
	}

	/* cierra sockets */
	close(cfd);
	close(sfd);

	return 0;
}
