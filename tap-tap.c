#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tap.h"

static void die (const char *reason)
{
	fflush (stdout);
	perror ("tap_alloc");
	exit (1);
}

int main (int argc, char* argv[])
{
	int fd0, fd1;
	char name[IFNAMSIZ + 1];

	fprintf (stderr, "Creating tap device - ");

	if ((fd0 = tap_alloc ("tap0", name)) == -1)
		die ("tap0 allocation");

	fprintf (stderr, "%s, ok\n", name);

	fprintf (stderr, "Creating tap device - ");

	if ((fd1 = tap_alloc ("tap1", name)) == -1 ) {
		close (fd0);
		die ("tap1 allocation");
	}

	fprintf (stderr, "%s, ok\n", name);

/*	x_link (fd0, fd1); */
	pause ();

	close (fd1);
	close (fd0);

	return 0;
}
