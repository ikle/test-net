#include <stdio.h>
#include <stdlib.h>

#include "net.h"
#include "tls.h"

static void die (const char *msg)
{
	fprintf (stderr, "tls-cli: %s\n", msg);
	exit (1);
}

int main (int argc, char *argv[])
{
	struct tls_client_cred *cred;
	int fd;
	struct tls *c;

	ssize_t ret;
	char buf[BUFSIZ];

	tls_init ();

	cred = tls_client_cred_open ("user", "0123456789ABCDEF");
	if (cred == NULL)
		die ("cannot initialize credentials");

	if ((fd = net_connect (SOCK_STREAM, "0", "12345")) < 0)
		die ("cannot open raw connection");

	if ((c = tls_open (fd, cred, 0)) == NULL)
		die ("cannot open TLS connection");

	/* receive data from server */
	do {
		if ((ret = tls_recv (c, buf, sizeof (buf))) < 0)
			die ("cannot receive data");

		fwrite (buf, ret, 1, stdout);
	}
	while (ret > 0);

	return 0;
}
