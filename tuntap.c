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

#include "tuntap.h"

int tuntap_alloc (const char *template, char *name)
{
	int fd;
	struct ifreq ifr;

	if ((fd = open ("/dev/net/tun", O_RDWR)) == -1)
		return -1;

	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

	if (template == NULL || *template == '\0')
		strncpy (ifr.ifr_name, "tap%d", IFNAMSIZ);
	else
		strncpy (ifr.ifr_name, template, IFNAMSIZ);

	if (ioctl (fd, TUNSETIFF, &ifr) == -1) {
		int err = errno;	/* save errno */
		close (fd);			/* from close error */
		errno = err;		/* & restore it */
		return -1;
	}

	if (name != NULL) {
		strncpy (name, ifr.ifr_name, IFNAMSIZ);
		name[IFNAMSIZ] = '\0';
	}

	return fd;
}
