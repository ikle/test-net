#ifndef ROUTE_H
#define ROUTE_H  1

struct route_entry {
	char dev[16];
	unsigned long dst, mask, gw;
	int flags, refs, use, metric, mtu, window, irtt;
};

int route_scan (int (*cb)(void *ctx, struct route_entry *e), void *ctx);

#endif  /* ROUTE_H */
