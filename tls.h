#ifndef TLS_H
#define TLS_H  1

/* library initialization and finalization */
void tls_init (void);
void tls_fini (void);

/* server credentials */
struct tls_server_cred *tls_server_cred_open (const char *password_file);
void tls_server_cred_close (struct tls_server_cred *c);

/* client credentials */
struct tls_client_cred *tls_client_cred_open (const char *name,
					      const char *key);
void tls_client_cred_close (struct tls_client_cred *c);

/* connection object */
struct tls *tls_open (int fd, void *cred, int passive);
void tls_close (struct tls *c);

ssize_t tls_send (struct tls *c, const void *data, size_t size);
ssize_t tls_recv (struct tls *c, void *data, size_t size);

#endif  /* TLS_H */
