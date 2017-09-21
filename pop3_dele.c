/* function pop3_dele - delete certain message */

#include "showmail.h"

int pop3_dele ( int this_socket , int index )
{	
	char delestring [MAXSTRING] ;
	char buffer [MAXBUF] ;

	/* send dele command and read response */
		
	sprintf ( delestring , "dele %d" , index ) ;

	if ( is_pop3s )
	{
		write_ssl ( this_ssl_connection , delestring ) ;
		read_ssl ( this_ssl_connection , buffer , sizeof ( buffer ) ) ;
	}
	else
	{
		write_socket ( this_socket , delestring ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
	}

	/* was it okay? */
	
	if ( strncmp ( buffer , "+OK" , 3 ) == 0 )
		return POP3_SUCCESS ;
	else
		return POP3_ERROR ;
}
