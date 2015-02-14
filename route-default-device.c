#include <stdio.h>

#include "route.h"

int main (int argc, char *argv[])
{
	char dev[16];

	if (!route_get_default_route_device (dev, sizeof (dev))) {
		perror ("route-default-device");
		return 1;
	}

	printf ("default route device: %s\n", dev);
	return 0;
}
