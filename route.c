#include <errno.h>
#include <stdio.h>

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
