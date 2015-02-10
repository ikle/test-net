#ifndef __TAP_H
#define __TAP_H 1

#include <net/if.h>		/* IF_NAMESIZE/IFNAMESIZ */

/*
 * DESCRIPTION: allocate tap tunnel
 * RETURN VALUE:
 *	file descriptor of tap tunnel back side or
 *	-1 on error, see errno
 *
 * if name == NULL th use default "tap%d"
 * Examples:
 *	"tapu-tapu%d" - дать системе выбрать имя по нашему шаблону
 *	"tap13" - точное указание имени
 *
 * if ret_name != NULL th return allocated name here, max IF_NAMESIZE chars
 *
 * NOTE: MAC address of tap device is 00:FF:ab:cd:ef:gh, where a-h random numbers
 */
int tap_alloc (const char *name, char *ret_name);

#endif /* __TAP_H */
