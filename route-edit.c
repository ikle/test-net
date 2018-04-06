#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "route.h"

in_addr_t inet_mask (int cidr_class)
{
	return ~0UL << (32 - cidr_class);
}

int main (int argc, char *argv[])
{
	struct sockaddr_in a;
	const char *dev = "eth0";

	memset (&a, 0, sizeof (a));

	a.sin_family = AF_INET;
	a.sin_addr.s_addr = inet_addr ("123.45.67.89");

	if (!route_add ((void *) &a, NULL, NULL, dev))
		goto error;

	sleep (5);

	if (!route_del ((void *) &a, NULL, NULL, dev))
		goto error;

	return 0;
error:
	perror ("route-edit");
	return 1;
}
