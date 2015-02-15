#include <errno.h>
#include <stdio.h>
#include "netif.h"

int main (int argc, char *argv[])
{
	char buf[32];

	if (argc != 2) {
		fprintf (stderr, "usage:\n\tnetif-info <interface-name>\n");
		return 1;
	}

	if (!netif_get_address_string (argv[1], buf, sizeof (buf))) {
		if (errno == EADDRNOTAVAIL) {
			printf ("%s: address not set\n", argv[1]);
			return 0;
		}

		perror ("netif-info");
		return 1;
	}

	printf ("%s: %s\n", argv[1], buf);
	return 0;
}
