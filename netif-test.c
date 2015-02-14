#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <unistd.h>

#ifdef __GNUC__
#define thread  __thread
#else
#define thread
#endif

static void die (const char *msg)
{
	perror (msg);
	exit (1);
}

const char *ether_ntoa (void *data)
{
	static thread char buf[18];
	unsigned char *mac = data;

	snprintf (buf, sizeof (buf), "%02x:%02x:%02x:%02x:%02x:%02x",
		  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return buf;
}

int main (int argc, char *argv[])
{
	int s;
	struct if_nameindex *list, *p;
	struct ifreq  ir, buf[16];
	struct ifconf ic;
	int i, count;

//	if ((s = socket (AF_LOCAL, SOCK_DGRAM, 0)) == -1)
	if ((s = socket (AF_INET, SOCK_DGRAM, 0)) == -1)  /* need AF_INET for get address */
		die ("UNIX socket");

	if ((list = if_nameindex ()) == NULL)
		die ("interface name index");

	for (p = list; p->if_index > 0; ++p) {
		printf ("%d. %s: ", p->if_index, p->if_name);

		strncpy (ir.ifr_name, p->if_name, IFNAMSIZ);

		if (ioctl (s, SIOCGIFFLAGS, &ir) == -1)
			die ("get if flags");

		if (ir.ifr_flags & IFF_UP)
			printf ("up ");

		if (ir.ifr_flags & IFF_LOOPBACK)
			printf ("loopback ");

//		ir.ifr_ifindex = p->if_index;
//		strncpy (ir.ifr_name, p->if_name, IFNAMSIZ);

		if (ioctl (s, SIOCGIFADDR, &ir) != -1)
			printf (": %s", inet_ntoa (((struct sockaddr_in *) &ir.ifr_addr)->sin_addr));

		if (ioctl (s, SIOCGIFHWADDR, &ir) != -1 &&
		    ir.ifr_hwaddr.sa_family == ARPHRD_ETHER)
			printf (": %s", ether_ntoa (ir.ifr_hwaddr.sa_data));

		printf ("\n");
	}

	if_freenameindex (list);

	printf ("\n");

	ic.ifc_len = sizeof (buf);
	ic.ifc_req = buf;

	if (ioctl (s, SIOCGIFCONF, &ic) == -1)
		die ("get if conf");

	for (count = ic.ifc_len / sizeof (buf[0]), i = 0; i < count; ++i)
		if (ic.ifc_req[i].ifr_addr.sa_family == AF_INET)
		printf ("%s: %s\n",
			ic.ifc_req[i].ifr_name,
			inet_ntoa (((struct sockaddr_in *) &ic.ifc_req[i].ifr_addr)->sin_addr));

	close (s);
	return 0;
}
