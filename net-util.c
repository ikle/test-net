#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "net-util.h"

int net_address_aton (const char *from, struct sockaddr *address,
		      struct sockaddr *mask)
{
	unsigned a, b, c, d, m;
	struct sockaddr_in *sa = (void *) address, *sm = (void *) mask;
	int n;

	n = sscanf (from, "%u.%u.%u.%u/%u", &a, &b, &c, &d, &m);

	if (n < 4 || a > 255 || b > 255 || c > 255 || d > 255 ||
	    (n == 5 && m > 32) || n > 5) {
		errno = EINVAL;
		return 0;
	}

	sa->sin_family = AF_INET;
	sa->sin_addr.s_addr = a | b << 8 | c << 16 | d << 24;

	if (n == 5 && mask != NULL) {
		sm->sin_family = AF_INET;
		sm->sin_addr.s_addr = htonl (0xffffffffL << (32 - m));
	}

	return n == 5 ? 2 : 1;
}
