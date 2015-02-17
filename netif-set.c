#include <errno.h>
#include <stdio.h>
#include "netif.h"

int main (int argc, char *argv[])
{
	if (argc != 3) {
		fprintf (stderr, "usage:\n\tnetif-set <interface-name> "
				 "<cidr-address/mask>\n");
		return 1;
	}

	if (!netif_set_address_from_string (argv[1], argv[2])) {
		perror ("netif-set");
		return 1;
	}

	return 0;
}
