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

int tuntap_alloc (const char *template, char *name, size_t size)
{
	int fd;
	struct ifreq ifr;

	if ((fd = open ("/dev/net/tun", O_RDWR)) == -1)
		return -1;

	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

	if (template == NULL || *template == '\0')
		template = "tap%d";

	snprintf (ifr.ifr_name, IFNAMSIZ, "%s", template);

	if (ioctl (fd, TUNSETIFF, &ifr) == -1) {
		close (fd);
		return -1;
	}

	if (name != NULL)
		snprintf (name, size, "%s", ifr.ifr_name);

	return fd;
}
