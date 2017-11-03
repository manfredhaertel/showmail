/* function pop3_top - get header lines for a certain message */

#include "showmail.h"

int pop3_top ( int this_socket , int index , char *header )
{	
	char topstring [MAXSTRING] ;
	char buffer [MAXBUF] ;
	int status ;

	/* ask for the headers */
		
	sprintf ( topstring , "top %d 0" , index ) ;

	if ( is_pop3s )
		write_ssl ( this_ssl_connection , topstring ) ;
	else
		write_socket ( this_socket , topstring ) ;

        status = read_response ( this_socket , this_ssl_connection , is_pop3s ,
                 buffer , NULL , "\r\n.\r\n" , "-ERR" , NULL ) ;
        if ( status == RESPONSE_ERROR )
                return POP3_ERROR ;
                                
	strcpy ( header , buffer ) ;
	
	return POP3_SUCCESS ;
}
