#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/route.h>
#include <unistd.h>

static int route_fn (struct sockaddr *to, struct sockaddr *mask,
		     struct sockaddr *via, const char *dev, int fn)
{
	int s, status = 1;
	struct rtentry e;

	if ((s = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
		return 0;

	memset (&e, 0, sizeof (e));
	memcpy (&e.rt_dst, to, sizeof (*to));

	if (mask != NULL)
		memcpy (&e.rt_genmask, mask, sizeof (*mask));
	else
		e.rt_flags |= RTF_HOST;

	if (via != NULL) {
		memcpy (&e.rt_gateway, via, sizeof (*via));
		e.rt_flags += RTF_GATEWAY;
	}

	e.rt_dev = (char *) dev;
	e.rt_flags |= RTF_UP;

	if (ioctl (s, fn, &e) == -1)
		status = 0;

	close (s);
	return status;
}

int route_add (struct sockaddr *to, struct sockaddr *mask,
	       struct sockaddr *via, const char *dev)
{
	return route_fn (to, mask, via, dev, SIOCADDRT);
}

int route_del (struct sockaddr *to, struct sockaddr *mask,
	       struct sockaddr *via, const char *dev)
{
	return route_fn (to, mask, via, dev, SIOCDELRT);
}

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
