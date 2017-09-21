/* function pop3_close - shutdown connection to pop server */

#include "showmail.h"

void pop3_close ( int this_socket )
{
	char buffer [MAXBUF] ;

	/* be polite, say goodbye */

	if ( is_pop3s )
	{
		write_ssl ( this_ssl_connection , "quit" ) ;
		read_ssl ( this_ssl_connection , buffer , sizeof ( buffer ) ) ;
	}
	else
	{
		write_socket ( this_socket , "quit" ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
	}
	
	/* close ssl connection if we have one */
	
	if ( is_pop3s )
		close_ssl ( this_ssl_connection , this_ssl_context ) ;
	
	/* close socket */

	close ( this_socket ) ;
}	 
