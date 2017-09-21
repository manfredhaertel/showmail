/* write_socket - write to a socket */

#include "showmail.h"

void write_socket ( int this_socket , char *buffer )
{
	int size_of_buffer ;
	int number_of_bytes ;
	char this_buffer [256] ;
	
	/* append cr/lf to the string - note that some servers would be
           satisfied only with lf, but not all */

	strncpy ( this_buffer , buffer , sizeof ( this_buffer ) ) ;
	strcat ( this_buffer , "\r\n" ) ;

	/* actually write */

	size_of_buffer = strlen ( this_buffer ) ;
	
	number_of_bytes = write ( this_socket , this_buffer , size_of_buffer ) ;
	
	/* error check */

	if ( number_of_bytes <= 0 )
	{
		if ( debug )
			printf ( "cannot write to socket.\n" ) ;
	}
	
	/* echo if debug */

	if ( debug )
		printf ( "S: %s" , this_buffer ) ;
}			          
