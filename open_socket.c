/* function open_socket - open a socket to a certain service on a certain
   node */

#include "showmail.h"

int open_socket ( char *node_name , char *service_name )
{
	struct servent *this_service ;
	int this_port ;
	struct hostent *this_host ;
	struct in_addr *this_internet_address ;
	int this_socket ;
	struct sockaddr_in this_socket_address ;
	int connected ;

	/* get service name */

	this_service = getservbyname ( service_name , "tcp" ) ;
	
	if ( this_service != NULL )
	{
		this_port = this_service->s_port ;
	}
	else
	{
		if ( ( atoi ( service_name ) > 0 ) &&
		     ( atoi ( service_name ) < 65536 ) )
		{
			this_port = htons ( atoi ( service_name ) ) ;
		}
		else
		{
			/* /etc/services failed, try hard coded */
			
			if ( strcmp ( service_name , "pop3" ) == 0 )
				this_port = htons ( 110 ) ;
			else if ( strcmp ( service_name , "pop3s" ) == 0 )
				this_port = htons ( 995 ) ;
			else if ( strcmp ( service_name , "imap" ) == 0 )
				this_port = htons ( 143 ) ;
			else if ( strcmp ( service_name , "imaps" ) == 0 )
				this_port = htons ( 993 ) ;
			else
			{
				if ( debug )
					printf ( "service not found.\n" ) ;
				return SHOWMAIL_SOCKET_ERROR ;
			}
		}
	}
	
	/* get host name */
	
	this_host = gethostbyname ( node_name ) ;
	if ( this_host == NULL )
	{
		if ( debug )
			printf ( "node not found.\n" ) ;
		return SHOWMAIL_SOCKET_ERROR ;
	}
	
	this_internet_address = (struct in_addr* ) *this_host->h_addr_list ;

	/* open socket */
	
	this_socket = socket ( AF_INET , SOCK_STREAM , 0 ) ;
	
	if ( this_socket < 0 )
	{
		if ( debug )
			printf ( "cannot open socket.\n" ) ;
		return SHOWMAIL_SOCKET_ERROR ;
	}	
	
	/* connect */

	this_socket_address.sin_family = AF_INET ;
	this_socket_address.sin_port = this_port ;
	this_socket_address.sin_addr.s_addr = this_internet_address->s_addr ;
	
	connected = connect ( this_socket , 
	                      (struct sockaddr *) &this_socket_address , 
	                      sizeof ( this_socket_address ) ) ;
	                      
	/* error handling */
	
	if ( connected == SHOWMAIL_SOCKET_ERROR )
	{
		if ( debug )
			printf ( "connect failed.\n" ) ;
		return SHOWMAIL_SOCKET_ERROR ;
	}
	
	/* return file handle */
	
	return this_socket ;
}	          
