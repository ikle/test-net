#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef uint_least32_t u32;

static int ip_cmp (const void *A, const void *B)
{
	const u32 *a = A, *b = B;

	return *a - *b;
}

static int netmask_to_cidr (u32 m)
{
	static u32 table[32] = {
		0x80000000, 0xc0000000, 0xe0000000, 0xf0000000,
		0xf8000000, 0xfc000000, 0xfe000000, 0xff000000,
		0xff800000, 0xffc00000, 0xffe00000, 0xfff00000,
		0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000,
		0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000,
		0xfffff800, 0xfffffc00, 0xfffffe00, 0xffffff00,
		0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0,
		0xfffffff8, 0xfffffffc, 0xfffffffe, 0xffffffff,
	};
	u32 *p;

	m = ntohl (m);
	p = bsearch (&m, table, 32, sizeof (table[0]), ip_cmp);
	if (p == NULL) {
		errno = EINVAL;
		return -1;
	}

	return p - table + 1;
}

struct route_entry {
	char dev[16];
	unsigned long dst, mask, gw;
	int flags, refs, use, metric, mtu, window, irtt;
};

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

static int route_print_cb (void *ctx, struct route_entry *e)
{
	FILE *f = ctx;
	struct in_addr dst, gw;

	dst.s_addr = e->dst;
	gw.s_addr  = e->gw;

	if (dst.s_addr == INADDR_ANY)
		fprintf (f, "default");
	else {
		fprintf (f, "%s", inet_ntoa (dst));

		if (e->mask != INADDR_BROADCAST)
			fprintf (f, "/%d", netmask_to_cidr (e->mask));
	}

	if (gw.s_addr != INADDR_ANY)
		fprintf (f, " via %s", inet_ntoa (gw));

	fprintf (f, " dev %s\n", e->dev);

	return 1;
}

int main (int argc, char *argv[])
{
	if (!route_scan (route_print_cb, stdout)) {
		perror ("route-list");
		return 1;
	}

	return 0;
}
