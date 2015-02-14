#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <unistd.h>

#include "route.h"

int route_scan (int (*cb)(void *ctx, struct route_entry *e), void *ctx)
{
	FILE *f;
	struct route_entry e;
	int n;

	if ((f = fopen ("/proc/net/route", "r")) == NULL)
		return 0;

	/* skip header */
	while (!feof (f) && fgetc (f) != '\n') {}

	while (!feof (f)) {
		n = fscanf (f, "%15s %lx %lx %x %d %d %d %lx %d %d %d \n",
			    e.dev, &e.dst, &e.gw, &e.flags, &e.refs, &e.use,
			    &e.metric, &e.mask, &e.mtu, &e.window, &e.irtt);
		if (n != 11) {
			errno = EPROTO;
			goto out;
		}

		if (!cb (ctx, &e))
			break;

	}
out:
	fclose (f);
	return 1;
}

struct route_buf {
	char *data;
	size_t size;
};

static int route_default_route_device_cb (void *ctx, struct route_entry *e)
{
	struct route_buf *dev = ctx;
	struct in_addr dst;

	dst.s_addr = e->dst;

	if (dst.s_addr == INADDR_ANY) {
		snprintf (dev->data, dev->size, "%s", e->dev);
		return 0;
	}

	return 1;
}

int route_get_default_route_device (char *buf, size_t size)
{
	struct route_buf dev = {buf, size};

	if (!route_scan (route_default_route_device_cb, &dev))
		return 0;

	return 1;
}

static int route_fn (struct sockaddr *to, struct sockaddr *mask,
		     struct sockaddr *via, const char *dev, int fn)
{
	int s, status = 1;
	struct rtentry e;

	if ((s = socket (to->sa_family, SOCK_DGRAM, 0)) == -1)
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
