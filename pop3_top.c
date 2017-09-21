/* function pop3_top - get header lines for a certain message */

#include "showmail.h"

int pop3_top ( int this_socket , int index , char *header )
{	
	char topstring [MAXSTRING] ;
	char buffer [MAXBUF] ;
	char endofbuffer [6] ;

	/* ask for the headers */
		
	sprintf ( topstring , "top %d 0" , index ) ;

	if ( is_pop3s )
		write_ssl ( this_ssl_connection , topstring ) ;
	else
		write_socket ( this_socket , topstring ) ;

	/* we read until a dot comes */

	strcpy ( buffer , "" ) ;
		
	do
	{
		if ( is_pop3s )
			read_ssl ( this_ssl_connection , 
                                   buffer + strlen ( buffer ) , 
                                   sizeof ( buffer ) - strlen ( buffer ) ) ;
                else
			read_socket ( this_socket , 
                                      buffer + strlen ( buffer ) , 
                                      sizeof ( buffer ) - strlen ( buffer ) ) ;
                              
                /* return error code if this command fails */
                
                if ( strncmp ( buffer , "-ERR" , 4 ) == 0 )
                	return POP3_ERROR ;

		strncpy ( endofbuffer , 
                          buffer + strlen ( buffer ) - 5 ,
                          5 ) ;
	}
	while ( ( strncmp ( endofbuffer , "\r\n.\r\n" , 5 ) != 0 ) && 
	        ( strlen ( buffer ) < MAXBUF - 1 ) ) ;
	
	strcpy ( header , buffer ) ;
	
	return POP3_SUCCESS ;
}
