/* function pop3_open - establish the connection to the pop server */

#include "showmail.h"

int pop3_open ( char *host , char *protocol , char *user , char *password )
{	
	int this_socket ;
	char buffer [MAXBUF] ;
	char userstring [MAXSTRING] ;
	char passwordstring [MAXSTRING] ;
	int ssl_okay ;
	char real_host [MAXSTRING] ;
	char real_protocol [MAXSTRING] ;
	int socks_status ;
	int stls ;

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
	
	if ( ( strcmp ( protocol , "pop3s" ) == 0 ) ||
	     ( strcmp ( protocol , "995"   ) == 0 ) ||
	     ( strcmp ( protocol , "spop3" ) == 0 ) )
	{
		is_pop3s = 1 ;
	}
	else
	{
		is_pop3s = 0 ;
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
	
	if ( ! is_pop3s )
	{
		/* read announcement */
		
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
		
		/* send stls command */
		
		write_socket ( this_socket , "stls" ) ;
		nanosleep ( (const struct timespec[]) {{ 0 , 100000000 }} , NULL ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
		
		if ( strncmp ( buffer , "+OK" , 3 ) == 0 )
		{
			printf ( "STLS succeeded, switching to SSL\n" ) ;
			is_pop3s = 1 ;
			stls = 1 ;
		}
		
		/* some silly servers disconnect, after they receive the
		   stls command. very special workaround for this,
		   simply connect again. */
		   
		if ( strstr ( buffer , "signing off" ) != NULL )
		{
			printf ( "Server %s disconnects after stls, trying to reconnect\n" , host ) ;
			close ( this_socket ) ;
			this_socket = open_socket ( host , protocol ) ;
			if ( this_socket == SHOWMAIL_SOCKET_ERROR )
			{
				if ( debug )
					printf ( "reconnect failed.\n" ) ;
				return SHOWMAIL_SOCKET_ERROR ;
			}
			read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
		}			
	}

	/* open ssl connection if needed */

	if ( is_pop3s )
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

	/* send the username */
	
	sprintf ( userstring , "user %s" , user ) ;
	if ( is_pop3s )
	{
		write_ssl ( this_ssl_connection , userstring ) ;
		read_ssl ( this_ssl_connection , buffer , sizeof ( buffer ) ) ;
	}
	else
	{
		write_socket ( this_socket , userstring ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
	}

	/* send the password */
		
	sprintf ( passwordstring , "pass %s" , password ) ;
	if ( is_pop3s )
	{
		write_ssl ( this_ssl_connection , passwordstring ) ;
		read_ssl ( this_ssl_connection , buffer , sizeof ( buffer ) ) ;
	}
	else
	{
		write_socket ( this_socket , passwordstring ) ;
		read_socket ( this_socket , buffer , sizeof ( buffer ) ) ;
	}

	/* if there is no okay message now, authentication failed. then
           it's time to say goodbye */

	if ( strncmp ( buffer , "+OK" , 3 ) != 0 )
	{
                printf ( "authentication failed.\n" ) ;
			
		pop3_close ( this_socket ) ;
	        
	        return SHOWMAIL_SOCKET_ERROR ;
	}
	
	return this_socket ;
}
