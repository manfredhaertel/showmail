/* function imap_delete - delete certain message */

#include "showmail.h"

int imap_delete ( int this_socket , int index )
{
	char storestring [MAXSTRING] ;
	char okstring [MAXSTRING] ;
	char errorstring1 [MAXSTRING] ;
	char errorstring2 [MAXSTRING] ;
	char buffer [MAXBUF] ;
	int status ;

	/* only set deleted flag here */

	sprintf ( storestring , "a%03d store %d +flags \\Deleted" , imap_seq_number , index ) ;
	sprintf ( okstring , "\r\na%03d OK " , imap_seq_number ) ;
	sprintf ( errorstring1 , "a%03d NO " , imap_seq_number ) ;
	sprintf ( errorstring2 , "a%03d BAD " , imap_seq_number ) ;
	imap_seq_number ++ ;

	if ( is_imaps )
		write_ssl ( this_ssl_connection , storestring ) ;
	else
		write_socket ( this_socket , storestring ) ;

        status = read_response ( this_socket , this_ssl_connection , is_imaps ,
                buffer , okstring , NULL , errorstring1 , errorstring2 ) ;
        if ( status == RESPONSE_ERROR )
                return IMAP_ERROR ;

	return IMAP_SUCCESS ;
}
