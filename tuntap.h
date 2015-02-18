#ifndef TUNTAP_H
#define TUNTAP_H  1

/*
 * Allocate tuntap tunnel device. Return the new file descriptor of tunnel
 * back side, or -1 if an error occurred (in which case, errno is set
 * appropriately).
 *
 * template   -- device name template, if null then use defaul one;
 * ll         -- create tunnel device with link layer or not;
 * name, size -- output buffer for real device name.
 */
int tuntap_alloc (const char *template, int ll, char *name, size_t size);

#endif /* TUNTAP_H */
