#ifndef TUNTAP_H
#define TUNTAP_H  1

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
int tuntap_alloc (const char *template, char *name, size_t size);

#endif /* TUNTAP_H */
