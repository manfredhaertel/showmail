#include "showmail.h"

void close_ssl ( SSL *ssl_connection , SSL_CTX *ssl_context )
{
	SSL_shutdown ( ssl_connection ) ;
	SSL_free ( ssl_connection ) ;
	SSL_CTX_free ( ssl_context ) ;
}
