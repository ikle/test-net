#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>

#include "netif.h"

static int netif_request (const char *dev, int cmd, struct ifreq *ir)
{
	int s;

	if ((s = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
		goto no_socket;

	strncpy (ir->ifr_name, dev, IFNAMSIZ);

	if (ioctl (s, cmd, ir) == -1)
		goto no_ioctl;

	close (s);
	return 1;
no_ioctl:
	close (s);
no_socket:
	return 0;
}

static int netif_request_addr (const char *dev, int cmd,
			       struct sockaddr *addr)
{
	struct ifreq ir;

	if (!netif_request (dev, cmd, &ir))
		return 0;

	*addr = ir.ifr_addr;
	return 1;
}

int netif_get_address (const char *dev, struct sockaddr *addr)
{
	return netif_request_addr (dev, SIOCGIFADDR, addr);
}

int netif_get_netmask (const char *dev, struct sockaddr *mask)
{
	return netif_request_addr (dev, SIOCGIFNETMASK, mask);
}

int netif_get_hwaddress (const char *dev, struct sockaddr *hwaddr)
{
	return netif_request_addr (dev, SIOCGIFHWADDR, hwaddr);
}
