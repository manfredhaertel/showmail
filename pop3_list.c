/* function pop3_list - get size information for a certain message */

#include "showmail.h"

int pop3_list ( int this_socket , int index , int *size )
{	
	char liststring [MAXSTRING] ;
	char buffer [MAXBUF] ;
	int counter ;
	int offset ;

	/* ask for the headers and read answer */
		
	sprintf ( liststring , "list %d" , index ) ;
	
	if ( is_pop3s )
	{
		write_ssl ( this_ssl_connection , liststring ) ;
		read_ssl ( this_ssl_connection , buffer , sizeof ( buffer ) ) ;
	}
        else
        {                      
		write_socket ( this_socket , liststring ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
	}
                              
        /* command successfull? */
                
        if ( strncmp ( buffer , "+OK" , 3 ) == 0 )
	{
		/* sucess: size information comes after second blank */
		
		offset = 0 ;

		for ( counter = 1 ; counter <= 2 ; counter ++ )
		{
			while ( buffer [offset] != ' ' )
				offset ++ ;
			offset ++ ;
		}
				
		*size = atoi ( buffer + offset ) ;
		
		return POP3_SUCCESS ;
	}
	else
	{
		/* error: just signal error code */
		
		return POP3_ERROR ;
	}
}
