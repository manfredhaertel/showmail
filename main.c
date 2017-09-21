/* main.c for showmail */

#include "showmail.h"


/* main function */

int main ( int argc , char **argv )
{
	int this_socket ;
	int number_of_mails ;
	int total_size ;
	int counter ;
	char header [MAXBUF] ;
	char this_header [MAXBUF] ;
	char really_delete [MAXSTRING] ;
	int delete_status ;
	int top_status ;
	int list_status ;
	int size ;
	char server [MAXSTRING] ;
	char protocol [MAXSTRING] ;
	char user [MAXSTRING] ;
	char password [MAXSTRING] ;
	char feature [MAXSTRING] ;

	/* if there is any sixth parameter then assume that debugging
           is desired */	

	debug = ( argc == 7 ) ;
	set_debug ( debug ) ;
	
	/* announce */
	
	printf ( "%s V%s by Manfred Haertel\n\n" , PACKAGE_NAME , 
		  PACKAGE_VERSION ) ;

	/* get parameter */
	
	get_parameter ( argc , argv , 
	                server , protocol , user , password , feature ) ;

	/* establish connection */

	this_socket = pop3_open ( server , protocol , user , password ) ;

	/* message if this does not work */
	
	if ( this_socket == SHOWMAIL_SOCKET_ERROR )
	{
		printf ( "could not establish connection to %s server %s\n" ,
		          protocol , server ) ;
		exit ( 1 ) ;
	}
	
	/* get number of mails */
	
	pop3_stat ( this_socket , &number_of_mails , &total_size ) ;
	
	printf ( "\n%d message(s) with a total of %d bytes are waiting\n" ,
		 number_of_mails , total_size ) ;

	/* get headers and display items */
		 
        for ( counter = 1 ; counter <= number_of_mails ; counter++ )
        {
		top_status = pop3_top ( this_socket , counter , header ) ;
		
		if ( top_status == POP3_SUCCESS )
		{
                	printf("\nMessage #%d:\n" , counter ) ;
                
                	scan_header ( header , "From" , this_header ) ;
                	if ( strlen ( this_header ) == 0 )
                		strcpy ( this_header , "From: ???" ) ;
                	printf("  %s\n" , this_header ) ;

                	scan_header ( header , "Subject" , this_header ) ;
                	if ( strlen ( this_header ) == 0 )
                		strcpy ( this_header , "Subject: ???" ) ;
                	printf("  %s\n" , this_header ) ;

                	scan_header ( header , "Date" , this_header ) ;
                	if ( strlen ( this_header ) == 0 )
                		strcpy ( this_header , "Date: ???" ) ;
                	printf("  %s\n" , this_header ) ;
                	
                	list_status = pop3_list ( this_socket , 
                	                          counter , &size ) ;
                	
                	if ( list_status == POP3_SUCCESS )
                		printf ( "  Size: %d bytes\n" , size ) ;
                	else
                		printf ( "  Size: ???\n" ) ;
                
                	if ( feature[0] == 'd' )
                	{
                		printf ( "Delete this message? (y/[n]) " ) ;
                		scanf ( "%s" , really_delete ) ;
                		if ( really_delete [0] == 'y' )
                		{
                			delete_status = 
                			   pop3_dele ( this_socket , counter ) ;
                			
                			if ( delete_status == POP3_SUCCESS )
                			       printf ( "Message deleted!\n" ) ;
                			else
                				printf 
                		       ( "Message could NOT be deleted!!!\n" ) ;
                		}
                	}
                }
                else
                {
                	/* if top does not work, give an error message */
                	
                	printf ( "\nMessage #%d could not be obtained.\n" ,
                		  counter ) ;
                }
        }
        
        pop3_close ( this_socket ) ;

	exit ( 0 ) ;
}
