#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <net/if.h>

#include <fcntl.h>
#include <unistd.h>

#include <linux/if_tun.h>

#include "tap.h"

int tap_alloc (const char *name, char *ret_name)
{
	int fd;
	struct ifreq ifr;

	if ((fd = open ("/dev/net/tun", O_RDWR)) == -1)
		return -1;

/*	memset (&ifr, 0, sizeof (ifr)); */
	/*
		Flags:

		IFF_TUN   - TUN device (no Ethernet headers)
		IFF_TAP   - TAP device
		IFF_NO_PI - Do not provide packet information

		If flag IFF_NO_PI is not set each frame format is:
		Flags [2 bytes]
		Proto [2 bytes]
		Raw protocol (IP, IPv6, etc) frame.
	*/
	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

	if (name == NULL || *name == '\0')
		strncpy (ifr.ifr_name, "tap%d", IFNAMSIZ);
	else
		strncpy (ifr.ifr_name, name, IFNAMSIZ);

	if (ioctl (fd, TUNSETIFF, &ifr) == -1) {
		int err = errno;	/* save errno */
		close (fd);			/* from close error */
		errno = err;		/* & restore it */
		return -1;
	}

	if (ret_name != NULL) {
		strncpy (ret_name, ifr.ifr_name, IFNAMSIZ);
		ret_name[IFNAMSIZ] = '\0';
	}

	return fd;
}
