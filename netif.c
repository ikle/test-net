#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>

#include "netif.h"

static int netif_request (const char *dev, int cmd, struct ifreq *ir)
{
	int s;

	if ((s = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
		goto no_socket;

	snprintf (ir->ifr_name, IFNAMSIZ, "%s", dev);

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

static int netif_send_addr (const char *dev, int cmd,
			    const struct sockaddr *addr)
{
	struct ifreq ir;

	ir.ifr_addr = *addr;

	return netif_request (dev, cmd, &ir);
}

int netif_set_address (const char *dev, const struct sockaddr *addr)
{
	return netif_send_addr (dev, SIOCSIFADDR, addr);
}

int netif_set_netmask (const char *dev, const struct sockaddr *mask)
{
	return netif_send_addr (dev, SIOCSIFNETMASK, mask);
}

/* IPv4 network mask to CIDR format conversion helper */
typedef uint_least32_t u32;

static int ip_cmp (const void *A, const void *B)
{
	const u32 *a = A, *b = B;

	return *a - *b;
}

static int netmask_to_cidr (struct sockaddr *sa)
{
	static const u32 table[32] = {
		0x80000000, 0xc0000000, 0xe0000000, 0xf0000000,
		0xf8000000, 0xfc000000, 0xfe000000, 0xff000000,
		0xff800000, 0xffc00000, 0xffe00000, 0xfff00000,
		0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000,
		0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000,
		0xfffff800, 0xfffffc00, 0xfffffe00, 0xffffff00,
		0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0,
		0xfffffff8, 0xfffffffc, 0xfffffffe, 0xffffffff,
	};
	u32 m, *p;

	if (sa->sa_family != AF_INET)
		goto error;

	m = ntohl (((struct sockaddr_in *) sa)->sin_addr.s_addr);
	p = bsearch (&m, table, 32, sizeof (table[0]), ip_cmp);
	if (p == NULL)
		goto error;

	return p - table + 1;
error:
	errno = EINVAL;
	return -1;
}

int netif_get_address_string (const char *dev, char *buf, size_t size)
{
	struct sockaddr addr, mask;

	if (!netif_get_address (dev, &addr) ||
	    !netif_get_netmask (dev, &mask))
		return 0;

	return snprintf (buf, size, "%s/%d",
			 inet_ntoa (((struct sockaddr_in *) &addr)->sin_addr),
			 netmask_to_cidr (&mask));
}

int netif_get_hwaddress_string (const char *dev, char *buf, size_t size)
{
	struct sockaddr addr;
	unsigned char *mac;

	if (!netif_get_hwaddress (dev, &addr))
		return 0;

	mac = addr.sa_data;

	return snprintf (buf, size, "%02x:%02x:%02x:%02x:%02x:%02x",
			 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
