/* function imap_close - shutdown connection to pop server */

#include "showmail.h"

void imap_close ( int this_socket )
{
	char logoutstring [MAXSTRING] ;
	char buffer [MAXBUF] ;

	/* be polite, say goodbye */

	sprintf ( logoutstring , "a%03d logout" , imap_seq_number ) ;

	if ( is_imaps )
	{
		write_ssl ( this_ssl_connection , logoutstring ) ;
		read_ssl ( this_ssl_connection , buffer , sizeof ( buffer ) ) ;
	}
	else
	{
		write_socket ( this_socket , logoutstring ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
	}
	
	/* close ssl connection if we have one */
	
	if ( is_imaps )
		close_ssl ( this_ssl_connection , this_ssl_context ) ;
	
	/* close socket */

	close ( this_socket ) ;
}	 
