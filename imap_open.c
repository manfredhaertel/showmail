/* function imap_open - establish the connection to the imap server */

#include "showmail.h"

int imap_open ( char *host , char *protocol , char *user , char *password , int *number_of_mails )
{	
	int this_socket ;
	char buffer [MAXBUF] ;
	char tlsstring [MAXSTRING] ;
	char loginstring [MAXSTRING] ;
	char selectstring [MAXSTRING] ;
	char okstring [MAXSTRING] ;
	char errorstring1 [MAXSTRING] ;
	char errorstring2 [MAXSTRING] ;
	int ssl_okay ;
	char real_host [MAXSTRING] ;
	char real_protocol [MAXSTRING] ;
	int socks_status ;
	int stls ;
	char *pointer ;
	char folder [MAXSTRING] ;
	int status ;

#ifdef HAVE_WINSOCK_H
	/* initialize winsock */
	
	WSADATA wsadata ;
	int wsaerr ;
	
	wsaerr = WSAStartup ( MAKEWORD ( 2 , 2 ) , &wsadata ) ;
	if ( wsaerr != 0 )
	{
		if ( debug )
			printf ( "no socket library available\n" ) ;
		return SHOWMAIL_SOCKET_ERROR ;
	}
#endif

	/* is this ssl? */
	
	if ( ( strcmp ( protocol , "imaps" ) == 0 ) ||
	     ( strcmp ( protocol , "993"   ) == 0 ) )
	{
		is_imaps = 1 ;
	}
	else
	{
		is_imaps = 0 ;
		printf ( "Connection WITHOUT encryption (not recommended)\n" ) ;
	}

	/* special case: socks */
	
	if ( strstr ( host , ">" ) != NULL )
	{
		strcpy ( real_host , strstr ( host , ">" ) + 1 ) ;
		*strstr ( host , ">" ) = '\0' ;
		
		/* port number ? */
		
		strcpy ( real_protocol , protocol ) ;
		
		if ( strstr ( host , ":" ) != NULL )
		{
			strcpy ( protocol , strstr ( host , ":" ) + 1 ) ;
			*strstr ( host , ":" ) = '\0' ;
		}
		else
		{
			/* default is socks, 1080 */
			
			strcpy ( protocol , "1080" ) ;
		}
	}
	else
	{
		strcpy ( real_host , "" ) ;
		strcpy ( real_protocol , "" ) ;
	}
	
	/* open socket */

	this_socket = open_socket ( host , protocol ) ;
	
	/* check if this worked */
	
	if ( this_socket == SHOWMAIL_SOCKET_ERROR )
	{
		if ( debug )
			printf ( "socket open failed.\n" ) ;

		return SHOWMAIL_SOCKET_ERROR ;
	}
	
	/* open socks connection if needed */

	if ( strlen ( real_host ) > 0 )
	{
		socks_status = open_socks5 ( this_socket , real_host , 
		                             real_protocol ) ;
		                             
		if ( socks_status != 0 )
		{
			if ( debug )
				printf ( "socks open failed with status %d.\n" ,
				          socks_status ) ;
			return SHOWMAIL_SOCKET_ERROR ;
		}
	}
	
	/* try to upgrade to ssl via the stls command */
	
	stls = 0 ;
	
	if ( ! is_imaps )
	{
		/* read announcement */
		
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
		
		/* send starttls command */
		
        	sprintf ( tlsstring , "a%03d starttls" , imap_seq_number ) ;
        	sprintf ( okstring , "a%03d OK " , imap_seq_number ) ;
        	imap_seq_number ++ ;

		write_socket ( this_socket , tlsstring ) ;
		nanosleep ( (const struct timespec[]) {{ 0 , 100000000 }} , NULL ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
		
		if ( strncmp ( buffer , okstring , strlen ( okstring ) ) == 0 )
		{
			printf ( "STARTTLS succeeded, switching to SSL\n" ) ;
			is_imaps = 1 ;
			stls = 1 ;
		}
	}

	/* open ssl connection if needed */

	if ( is_imaps )
	{
		if ( strlen ( real_host ) > 0 )
			ssl_okay = open_ssl ( this_socket , real_host ) ;
		else
			ssl_okay = open_ssl ( this_socket , host ) ;

		/* check this */

		if ( ssl_okay == SHOWMAIL_SOCKET_ERROR )
		{
			if ( debug )
				printf ( "ssl open failed.\n" ) ;
			return SHOWMAIL_SOCKET_ERROR ;
		}
		
		/* read announcement */
		
		if ( ! stls )
			read_ssl ( this_ssl_connection , buffer , 
			           sizeof ( buffer ) ) ;
	}

	/* folder given in the username */

	if ( strstr ( user , ">" ) != NULL )
	{
		strcpy ( folder , strstr ( user , ">" ) + 1 ) ;
		*strstr ( user , ">" ) = '\0' ;
        }
        else
        {
                /* Default is Inbox */
                
                strcpy ( folder , "Inbox" ) ;
        }

	/* send the username and password */
	
	sprintf ( loginstring , "a%03d login %s %s" , imap_seq_number , user , password ) ;
	sprintf ( okstring , "a%03d OK " , imap_seq_number ) ;
	imap_seq_number ++ ;
	if ( is_imaps )
	{
		write_ssl ( this_ssl_connection , loginstring ) ;
		read_ssl ( this_ssl_connection , buffer , sizeof ( buffer ) ) ;
	}
	else
	{
		write_socket ( this_socket , loginstring ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
	}

	/* if there is no okay message now, authentication failed. then
           it's time to say goodbye */

	if ( strncmp ( buffer , okstring , strlen ( okstring ) ) != 0 )
	{
                printf ( "authentication failed.\n" ) ;
			
		imap_close ( this_socket ) ;
	        
	        return SHOWMAIL_SOCKET_ERROR ;
	}
	
	/* select inbox (or other folder) */
	
	sprintf ( selectstring , "a%03d select %s" , imap_seq_number , folder ) ;
        sprintf ( okstring , "\r\na%03d OK " , imap_seq_number ) ;
        sprintf ( errorstring1 , "a%03d NO " , imap_seq_number ) ;                
        sprintf ( errorstring2 , "a%03d BAD " , imap_seq_number ) ;                
	imap_seq_number ++ ;
	
	if ( is_imaps )
	{
		write_ssl ( this_ssl_connection , selectstring ) ;
	}
	else
	{
		write_socket ( this_socket , selectstring ) ;
	}

	status = read_response ( this_socket , this_ssl_connection , is_imaps ,
	        buffer , okstring , NULL , errorstring1 , errorstring2 ) ;
        if ( status == RESPONSE_ERROR )
        {
                printf ( "selecting folder %s failed\n" , folder ) ;
                return SHOWMAIL_SOCKET_ERROR ;
        }

	/* get number of mails */
	
        pointer = strstr ( buffer , " EXISTS" ) ;
        if ( pointer == NULL )
                return SHOWMAIL_SOCKET_ERROR ;
        
        pointer -- ;
        while ( ( pointer > buffer ) && ( *pointer != ' ' ) )
                pointer -- ;
                
        *number_of_mails = atoi ( pointer + 1 ) ;
	
	return this_socket ;
}
