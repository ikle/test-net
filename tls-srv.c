#include <stdio.h>
#include <stdlib.h>

#include "net.h"
#include "tls.h"

static void die (const char *msg)
{
	fprintf (stderr, "tls-srv: %s\n", msg);
	exit (1);
}

int main (int argc, char *argv[])
{
	struct tls_server_cred *cred;
	int fd_passive, fd;
	struct tls *c;

	static const char hello[] = "Hello from tls-srv!\n";
	ssize_t ret;
	char buf[BUFSIZ];

	tls_init ();

	cred = tls_server_cred_open ("./tls-passwd");
	if (cred == NULL)
		die ("cannot initialize credentials");

	if ((fd_passive = net_listen (SOCK_STREAM, NULL, "12345")) < 0)
		die ("cannot listen connections");

	for (;;) {
		if ((fd = accept (fd_passive, NULL, NULL)) == -1)
			die ("cannot open raw connection");

		if ((c = tls_open (fd, cred, 1)) == NULL)
			die ("cannot open TLS connection");

		if (tls_send (c, hello, sizeof (hello) - 1) < 0)
			die ("cannot send data");

		tls_close (c);
		close (fd);
	}

	return 0;
}
