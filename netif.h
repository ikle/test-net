#ifndef NETIF_H
#define NETIF_H  1

#include <sys/types.h>
#include <sys/socket.h>

int netif_get_address (const char *dev, struct sockaddr *addr);
int netif_get_netmask (const char *dev, struct sockaddr *mask);
int netif_get_hwaddress (const char *dev, struct sockaddr *hwaddr);

int netif_set_address (const char *dev, const struct sockaddr *addr);
int netif_set_netmask (const char *dev, const struct sockaddr *mask);

/*
 * Upon successful return, these functions return the number of characters
 * printed (excluding the null byte used to end output to strings).
 */
int netif_get_address_string (const char *dev, char *buf, size_t size);
int netif_get_hwaddress_string (const char *dev, char *buf, size_t size);

#endif  /* NETIF_H */
