/* function imap_fetch_header - get header lines for a certain message */

#include "showmail.h"

int imap_fetch_header ( int this_socket , int index , char *header )
{	
	char fetchstring [MAXSTRING] ;
	char okstring [MAXSTRING] ;
	char errorstring1 [MAXSTRING] ;
	char errorstring2 [MAXSTRING] ;
	char buffer [MAXBUF] ;
	int status ;

	/* ask for the headers */
		
	sprintf ( fetchstring , "a%03d fetch %d body.peek[header.fields (From Subject Date)]" , imap_seq_number , index ) ;
	sprintf ( okstring , "\r\na%03d OK " , imap_seq_number ) ;
	sprintf ( errorstring1 , "a%03d NO " , imap_seq_number ) ;
	sprintf ( errorstring2 , "a%03d BAD " , imap_seq_number ) ;
	imap_seq_number ++ ;

	if ( is_imaps )
		write_ssl ( this_ssl_connection , fetchstring ) ;
	else
		write_socket ( this_socket , fetchstring ) ;

        status = read_response ( this_socket , this_ssl_connection , is_imaps ,
                buffer , okstring , NULL , errorstring1 , errorstring2 ) ;
        if ( status == RESPONSE_ERROR )
                return IMAP_ERROR ;

	strcpy ( header , buffer ) ;
	
	return IMAP_SUCCESS ;
}
