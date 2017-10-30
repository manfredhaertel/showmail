/* function imap_expunge - purge deleted message */

#include "showmail.h"

int imap_expunge ( int this_socket )
{
	char expungestring [MAXSTRING] ;
	char okstring [MAXSTRING] ;
	char errorstring1 [MAXSTRING] ;
	char errorstring2 [MAXSTRING] ;
	char buffer [MAXBUF] ;

	/* purge messages flagged as deleted */

	sprintf ( expungestring , "a%03d expunge" , imap_seq_number ) ;
	sprintf ( okstring , "a%03d OK " , imap_seq_number ) ;
	sprintf ( errorstring1 , "\r\na%03d NO " , imap_seq_number ) ;
	sprintf ( errorstring2 , "\r\na%03d BAD " , imap_seq_number ) ;
	imap_seq_number ++ ;

	if ( is_imaps )
		write_ssl ( this_ssl_connection , expungestring ) ;
	else
		write_socket ( this_socket , expungestring ) ;

	/* we read until OK comes */

	strcpy ( buffer , "" ) ;
		
	do
	{
		if ( is_imaps )
			read_ssl ( this_ssl_connection , 
                                   buffer + strlen ( buffer ) , 
                                   sizeof ( buffer ) - strlen ( buffer ) ) ;
                else
			read_socket ( this_socket , 
                                      buffer + strlen ( buffer ) , 
                                      sizeof ( buffer ) - strlen ( buffer ) ) ;
                              
                /* return error code if this command fails */
                
                if ( ( strstr ( buffer , errorstring1 ) != NULL ) ||
                     ( strstr ( buffer , errorstring2 ) != NULL ) ||
                     ( strncmp ( buffer , errorstring1 + 2 , strlen ( errorstring1 ) - 2 ) == 0 ) ||
                     ( strncmp ( buffer , errorstring2 + 2 , strlen ( errorstring2 ) - 2 ) == 0 ) )
                	return IMAP_ERROR ;
	}
	while ( ( strstr ( buffer , okstring ) == NULL ) && 
	        ( strlen ( buffer ) < MAXBUF - 1 ) ) ;

	return IMAP_SUCCESS ;
}
