/* function read_response - get response for pop3 or imap */

#include "showmail.h"

int read_response ( int this_socket , SSL *this_ssl_connection , int is_ssl , 
                    char *buffer , char *okstring_any , char *okstring_end , 
                    char *errorstring1 , char *errorstring2 )
{
        int ok_found ;

	memset ( buffer , 0 , MAXBUF ) ;
		
	do
	{
	        /* note that the string termination stuff is done by
	           the read_* routines, so we can (and should) give them 
	           the whole buffer size */
	           
		if ( is_ssl )
			read_ssl ( this_ssl_connection , 
                                   buffer + strlen ( buffer ) , 
                                   MAXBUF - strlen ( buffer ) ) ;
                else
			read_socket ( this_socket , 
                                      buffer + strlen ( buffer ) , 
                                      MAXBUF - strlen ( buffer ) ) ;

                /* return error code if this command fails in the sense
                   of pop3 or imap */
                   
                if ( ( errorstring1 != NULL ) && 
                     ( strncmp ( buffer , errorstring1 , strlen ( errorstring1 ) ) == 0 ) )
                        return RESPONSE_ERROR ;
                if ( ( errorstring2 != NULL ) && 
                     ( strncmp ( buffer , errorstring2 , strlen ( errorstring2 ) ) == 0 ) )
                        return RESPONSE_ERROR ;

                /* check if ok was found, which is one condition for the
                   end of the loop - implemented the most readable way */

                ok_found = 0 ;
                if ( okstring_any != NULL )
                {
                        if ( strstr ( buffer , okstring_any ) != NULL )
                                ok_found = 1 ;
                }
                else if ( okstring_end != NULL )
                {
                        if ( strlen ( buffer ) >= strlen ( okstring_end ) )
                        {
                                if ( strncmp ( buffer + strlen ( buffer ) - strlen ( okstring_end ) , 
                                     okstring_end , strlen ( okstring_end ) ) == 0 )
                                        ok_found = 1 ;
                        }
                }
	}
	while ( ( ok_found == 0 ) && ( strlen ( buffer ) < MAXBUF - 1 ) ) ;
        
        /* if the buffer isn't large enough, there's something terribly
           wrong, so give a message and exit completely */ 
           
        if ( strlen ( buffer ) == MAXBUF - 1 )
        {
                printf ( "The response to our command is way too large.\n" ) ;
                printf ( "This MAY come from a malicious mail or server.\n" ) ;
                printf ( "Exiting the whole program to save our lives...\n" ) ;
                exit ( 1 ) ;
        }
	
	return RESPONSE_SUCCESS ;
}
