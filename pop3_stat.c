/* function pop3_stat - get number of messages */

#include "showmail.h"

void pop3_stat ( int this_socket , int *number_of_mails , int *total_size )
{	
	char buffer [MAXBUF] ;

	/* request number of messages (and total message size)
	   from the server */

	if ( is_pop3s )
	{
		write_ssl ( this_ssl_connection , "stat" ) ;
		read_ssl ( this_ssl_connection , buffer , sizeof ( buffer ) ) ;
	}
	else
	{
		write_socket ( this_socket , "stat" ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
	}
	
	/* number of mail located right behind the okay message */

	*number_of_mails = atoi ( buffer + 4 ) ;
	
	/* total size after the next blank */
	
	*total_size = atoi ( strstr ( buffer + 4 , " " ) + 1 ) ;
}
