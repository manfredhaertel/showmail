/* function open_socks5 - connect to a socks 5 server if needed */

#include "showmail.h"

int open_socks5 ( int this_socket , char *dns_name , char *service_name )
{
	unsigned char this_buffer [100] ;
	int counter ;
	struct servent *this_service ;
	int port_number ;
	
	/* get service name */

	this_service = getservbyname ( service_name , "tcp" ) ;
	
	if ( this_service != NULL )
	{
		port_number = this_service->s_port ;
	}
	else
	{
		if ( ( atoi ( service_name ) > 0 ) &&
		     ( atoi ( service_name ) < 65536 ) )
		{
			port_number = htons ( atoi ( service_name ) ) ;
		}
		else
		{
			/* /etc/services failed, try hard coded */

                        if ( strcmp ( service_name , "pop3" ) == 0 )
				port_number = htons ( 110 ) ;
			else if ( strcmp ( service_name , "pop3s" ) == 0 )
				port_number = htons ( 995 ) ;
			else
			{
				if ( debug )
					printf ( "service not found.\n" ) ;
				return SHOWMAIL_SOCKET_ERROR ;
			}
		}
	}

	/* method negotiation */
	
	this_buffer [0] = 5 ; /* socks version */
	this_buffer [1] = 1 ; /* number of methods */
	this_buffer [2] = 0 ; /* no authentification */
	
	write ( this_socket , this_buffer , 3 ) ;
	read ( this_socket , this_buffer , 2 ) ;
	
	if ( this_buffer [1] == 255 )
		return 255 ;
	
	/* construct buffer */

	this_buffer [0] = 5 ; /* socks version */
	this_buffer [1] = 1 ; /* connect */
	this_buffer [2] = 0 ; /* reserved */
	this_buffer [3] = 3 ; /* domainname */

	this_buffer [4] = strlen ( dns_name ) ;
	
	for ( counter = 0 ; counter < strlen ( dns_name ) ; counter ++ )
	{
		this_buffer [5+counter] = dns_name [counter] ;
	}

	port_number = ntohs ( port_number ) ;

	this_buffer [5+strlen(dns_name)] = ( port_number >> 8 ) ;
	this_buffer [5+strlen(dns_name)+1] = ( port_number & 0xff ) ;

	/* send buffer */

	write ( this_socket , this_buffer , strlen ( dns_name ) + 7 ) ;
	
	/* read response */
	
	read ( this_socket , this_buffer , 100 ) ;

	return (int) ( this_buffer [1] ) ;
}
