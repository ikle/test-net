#ifndef NET_UTIL_H
#define NET_UTIL_H  1

#include <sys/types.h>
#include <sys/socket.h>

int net_address_aton (const char *from, struct sockaddr *address,
		      struct sockaddr *mask);

#endif  /* NET_UTIL_H */
