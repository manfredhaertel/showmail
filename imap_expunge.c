/* function imap_expunge - purge deleted message */

#include "showmail.h"

int imap_expunge ( int this_socket )
{
	char expungestring [MAXSTRING] ;
	char okstring [MAXSTRING] ;
	char errorstring1 [MAXSTRING] ;
	char errorstring2 [MAXSTRING] ;
	char buffer [MAXBUF] ;
	int status ;

	/* purge messages flagged as deleted */

	sprintf ( expungestring , "a%03d expunge" , imap_seq_number ) ;
	sprintf ( okstring , "\r\na%03d OK " , imap_seq_number ) ;
	sprintf ( errorstring1 , "a%03d NO " , imap_seq_number ) ;
	sprintf ( errorstring2 , "a%03d BAD " , imap_seq_number ) ;
	imap_seq_number ++ ;

	if ( is_imaps )
		write_ssl ( this_ssl_connection , expungestring ) ;
	else
		write_socket ( this_socket , expungestring ) ;
		
        status = read_response ( this_socket , this_ssl_connection , is_imaps ,
                buffer , okstring , NULL , errorstring1 , errorstring2 ) ;
        if ( status == RESPONSE_ERROR )
                return IMAP_ERROR ;

	return IMAP_SUCCESS ;
}
