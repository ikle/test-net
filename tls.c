#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <gnutls/gnutls.h>

#include "tls.h"

/* library initialization and finalization */
void tls_init (void)
{
	gnutls_global_init();
}

void tls_fini (void)
{
	gnutls_global_deinit ();
}

/* server credentials */
static gnutls_psk_server_credentials_t
tls_server_cred_to_gnutls (struct tls_server_cred *c)
{
	return (void *) c;
}

struct tls_server_cred *tls_server_cred_open (const char *password_file)
{
	gnutls_psk_server_credentials_t cred;

	if (gnutls_psk_allocate_server_credentials (&cred) != 0)
		goto no_cred;

	if (gnutls_psk_set_server_credentials_file (cred,
						    password_file) != 0)
		goto no_init_cred;

	return (void *) cred;
no_init_cred:
	gnutls_psk_free_server_credentials (cred);
no_cred:
	return NULL;
}

void tls_server_cred_close (struct tls_server_cred *c)
{
	gnutls_psk_free_server_credentials (tls_server_cred_to_gnutls (c));
}

/* client credentials */
static gnutls_psk_client_credentials_t
tls_client_cred_to_gnutls (struct tls_client_cred *c)
{
	return (void *) c;
}

struct tls_client_cred *tls_client_cred_open (const char *name,
					      const char *key)
{
	gnutls_psk_client_credentials_t cred;
	gnutls_datum_t k;

	if (gnutls_psk_allocate_client_credentials (&cred) != 0)
		goto no_cred;

	k.size = strlen (key);
	k.data = (void *) key;

	if (gnutls_psk_set_client_credentials (cred, name, &k,
					       GNUTLS_PSK_KEY_HEX) != 0)
		goto no_init_cred;

	return (void *) cred;
no_init_cred:
	gnutls_psk_free_client_credentials (cred);
no_cred:
	return NULL;
}

void tls_client_cred_close (struct tls_client_cred *c)
{
	gnutls_psk_free_client_credentials (tls_client_cred_to_gnutls (c));
}

/* connection object */
struct tls {
	gnutls_session_t session;
};

static ssize_t tls_data_push (gnutls_transport_ptr_t ptr,
			      const void *data, size_t size)
{
	return send (ptr - NULL, data, size, 0);
}

static ssize_t tls_data_pull (gnutls_transport_ptr_t ptr,
			      void *data, size_t size)
{
	return recv (ptr - NULL, data, size, 0);
}

struct tls *tls_open (int fd, void *cred, int passive)
{
	struct tls *c;
	int res;
	const char *error = NULL;

	if ((c = malloc (sizeof (*c))) == NULL)
		goto no_tls;

	if (gnutls_init (&c->session, passive ? GNUTLS_SERVER :
						GNUTLS_CLIENT) != 0)
		goto no_session;

//	gnutls_session_set_ptr (c->session, c);

	if (gnutls_credentials_set (c->session, GNUTLS_CRD_PSK, cred) != 0)
		goto no_set_cred;

	if (gnutls_priority_set_direct (c->session,
					"SECURE128:+PSK:+DHE-PSK",
					&error) != 0)
		goto no_priority;

	gnutls_transport_set_ptr (c->session, fd + NULL);
	gnutls_transport_set_push_function (c->session, tls_data_push);
	gnutls_transport_set_pull_function (c->session, tls_data_pull);

	while ((res = gnutls_handshake (c->session)) != 0)
		if (gnutls_error_is_fatal (res))
			goto no_handshake;

	return c;
no_handshake:
no_priority:
no_set_cred:
	gnutls_deinit (c->session);
no_session:
	free (c);
no_tls:
	return NULL;
}

void tls_close (struct tls *c)
{
	gnutls_bye (c->session, GNUTLS_SHUT_RDWR);
	gnutls_deinit (c->session);
}

ssize_t tls_send (struct tls *c, const void *data, size_t size)
{
	ssize_t res;

	do
		res = gnutls_record_send (c->session, data, size);
	while (res == GNUTLS_E_INTERRUPTED || res == GNUTLS_E_AGAIN);

	if (!gnutls_error_is_fatal (res))
		return res;

	errno = EPROTO;
	return -1;
}

ssize_t tls_recv (struct tls *c, void *data, size_t size)
{
	ssize_t res;

	do
		res = gnutls_record_recv (c->session, data, size);
	while (res == GNUTLS_E_INTERRUPTED || res == GNUTLS_E_AGAIN);

	if (!gnutls_error_is_fatal (res))
		return res;

	errno = EPROTO;
	return -1;
}
