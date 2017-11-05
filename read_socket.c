/* function read_socket - read from the socket */

#include "showmail.h"

void read_socket ( int this_socket , char *buffer , int size_of_buffer )
{
	int number_of_bytes ;
			          
	/* we need place for the termating zero */
	
	size_of_buffer -- ;

	/* actually read */

	number_of_bytes = DO_READ_SOCKET ( this_socket , buffer , size_of_buffer ) ;
	
	/* error check */

	if ( number_of_bytes <= 0 )
	{
		printf ( "reading from socket failed, terminating..." ) ;
		exit ( 1 ) ;
	}	
	
	/* zero terminate to have a clean c language string */

	buffer [ number_of_bytes ] = 0 ;
	
	/* echo it in the debug case */

	if ( debug )
		printf ( "R: %s" , buffer ) ;
}
