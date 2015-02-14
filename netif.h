#ifndef NETIF_H
#define NETIF_H  1

#include <sys/types.h>
#include <sys/socket.h>

int netif_get_address (const char *dev, struct sockaddr *addr);
int netif_get_netmask (const char *dev, struct sockaddr *mask);
int netif_get_hwaddress (const char *dev, struct sockaddr *hwaddr);

#endif  /* NETIF_H */
