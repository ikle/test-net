#ifndef ROUTE_H
#define ROUTE_H  1

#include <sys/types.h>
#include <sys/socket.h>

struct route_entry {
	char dev[16];
	unsigned long dst, mask, gw;
	int flags, refs, use, metric, mtu, window, irtt;
};

int route_scan (int (*cb)(void *ctx, struct route_entry *e), void *ctx);
int route_get_default_route_device (char *buf, size_t size);

int route_add (struct sockaddr *to, struct sockaddr *mask,
	       struct sockaddr *via, const char *dev);

int route_del (struct sockaddr *to, struct sockaddr *mask,
	       struct sockaddr *via, const char *dev);

#endif  /* ROUTE_H */
