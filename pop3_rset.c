/* function pop3_rset - reset the delete markers */

#include "showmail.h"

int pop3_rset ( int this_socket )
{	
	char rsetstring [MAXSTRING] ;
	char buffer [MAXBUF] ;

	/* send rset command and read response */
		
	sprintf ( rsetstring , "rset" ) ;
	
	if ( is_pop3s )
	{
		write_ssl ( this_ssl_connection , rsetstring ) ;
		read_ssl ( this_ssl_connection , buffer , sizeof ( buffer ) ) ;
	}
	else
	{
		write_socket ( this_socket , rsetstring ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
	}

	/* was it okay? */
	
	if ( strncmp ( buffer , "+OK" , 3 ) == 0 )
		return POP3_SUCCESS ;
	else
		return POP3_ERROR ;
}
