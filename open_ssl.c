#include "showmail.h"

/* open ssl connection */

int open_ssl ( int this_socket , char *server )
{
	const SSL_METHOD *ssl_method ;
	X509 *server_cert ;
	char *cert_subject ;
	char *cert_issuer ;
	struct timeval start_time ;
	struct timeval end_time ;
	int sec ;
	int usec ;
	int connected ;
	int verify_result ;
	char *next_slash ;
	int item_length ;
	char next_item [40] ;
	FILE *certfile ;
	char certfilename [MAXSTRING] ;
	char certfiledir [MAXSTRING] ;
	unsigned char fingerprint [20] ;
	unsigned int length ;
	int counter ;
	BIO *bio_out ;
	char *cert_subject_temp ;
	char *cert_issuer_temp ;
	
	if ( debug )
		gettimeofday ( &start_time , 0 ) ;

	SSL_library_init () ;

#ifdef HAVE_TLS_CLIENT_METHOD
	ssl_method = TLS_client_method () ;
#else
	ssl_method = SSLv23_client_method () ;
#endif
	
	this_ssl_context = SSL_CTX_new ( ssl_method ) ;
	
	if ( this_ssl_context == NULL )
	{
		if ( debug )
			printf ( "cannont create ssl context.\n" ) ;
		return SHOWMAIL_SOCKET_ERROR ;
	}

	/* set information about certificate validation (it seems that
	   has to be done BEFORE opening a connection) */

	SSL_CTX_load_verify_locations ( this_ssl_context , NULL ,
	                                SSL_CERTS ) ;
	SSL_CTX_set_default_verify_paths ( this_ssl_context ) ;
	
	/* actually connect */

	this_ssl_connection = (SSL *) SSL_new ( this_ssl_context ) ;

	if ( this_ssl_connection == NULL )
	{
		if ( debug )
			printf ( "cannot create SSL connection.\n" ) ;
		return SHOWMAIL_SOCKET_ERROR ;
	}

	SSL_set_fd ( this_ssl_connection , this_socket ) ;

	connected = SSL_connect ( this_ssl_connection ) ;
	
	if ( connected == -1 )
	{
		if ( debug )
			printf ( "cannot connect to SSL server.\n" ) ;
		return SHOWMAIL_SOCKET_ERROR ;
	}
	
	if ( debug )
	{
		gettimeofday ( &end_time , 0 ) ;
	
		/* measure time for ssl negotiation */
	
		sec = end_time.tv_sec - start_time.tv_sec ;
		usec = end_time.tv_usec - start_time.tv_usec ;
	
		if ( usec < 0 )
		{
			usec += 1000000 ;
			sec -- ;
		}
	
		printf ( "Time for SSL negotiation: %d.%06d sec\n" , 
		          sec , usec ) ;
	}
	
	/* ssl properties */
	
	/* cipher */
	
	if ( debug )
	{
		printf ( "Negotiated Cipher: %s\n" ,
		          SSL_get_cipher ( this_ssl_connection ) ) ;		                          
	}
	
	printf ( "Connection using %d bit encryption\n" ,
	          SSL_get_cipher_bits ( this_ssl_connection , NULL ) ) ;
	          
	/* certificate */

	server_cert = SSL_get_peer_certificate ( this_ssl_connection ) ;

	if ( server_cert == NULL )
	{
		if ( debug )
			printf ( "cannot open server certificate.\n" ) ;
		return SHOWMAIL_SOCKET_ERROR ;
	}
	
	/* store the certificate , but don't, if there is no home dir */
	
	if ( ( strlen ( homedir ) > 0 ) &&
	     ( strlen ( server ) > 0 ) &&
	     ( strlen ( homedir ) + strlen ( "/.showmail/" ) +
	       strlen ( server ) + strlen ( ".pem" ) < MAXSTRING - 1 ) )
	{
		sprintf ( certfiledir  , "%s/.showmail" , homedir ) ;
		sprintf ( certfilename , "%s/.showmail/%s.pem" , 
	        	  homedir , server ) ;

		certfile = fopen ( certfilename , "w" ) ;
	
		if ( certfile == NULL )
		{
#ifdef HAVE_TWO_ARG_MKDIR
			if ( mkdir ( certfiledir , 0700 ) == -1 )
#else
#ifdef HAVE_ONE_ARG_MKDIR
			if ( mkdir ( certfiledir ) == -1 )
#else
#error No mkdir routine implemented for your environment
#endif
#endif
			{
				printf ( "cannot save server certificate.\n" ) ;
			}
			else
			{
				certfile = fopen ( certfilename , "w" ) ;
			}
		}
	
		if ( certfile != NULL )
		{
			PEM_write_X509 ( certfile , server_cert ) ;
			fclose ( certfile ) ;
		}
	}
	
	/* subject and issuer */

	cert_subject = X509_NAME_oneline ( X509_get_subject_name ( server_cert ) ,
	                                   0 , 0 ) ;
	                                   
        cert_issuer = X509_NAME_oneline ( X509_get_issuer_name ( server_cert ) ,
                                          0 , 0 ) ;

        cert_subject_temp = malloc ( strlen ( cert_subject ) + 1 ) ;
        cert_issuer_temp = malloc ( strlen ( cert_issuer ) + 1 ) ;

	if ( ( strlen ( cert_subject ) == 0 ) ||
	     ( strlen ( cert_issuer  ) == 0 ) )
	{
		if ( debug )
			printf ( "cannot get certificate subject and/or issuer.\n" ) ;
		return SHOWMAIL_SOCKET_ERROR ;
	}

	strcpy ( cert_subject_temp , cert_subject + 1 ) ;
	strcpy ( cert_subject , cert_subject_temp ) ;
	strcpy ( cert_issuer_temp , cert_issuer + 1 ) ;
	strcpy ( cert_issuer , cert_issuer_temp ) ;

	printf ( "\n" ) ;
	printf ( "Certificate Subject:                    Certificate Issuer:\n" ) ;
	printf ( "====================                    ===================\n" ) ;
	
	while ( ( strlen ( cert_subject ) > 0 ) ||
	        ( strlen ( cert_issuer  ) > 0 ) )
	{
		/* subject */
		
		next_slash = strstr ( cert_subject , "/" ) ;
		
		if ( next_slash != NULL )
		{
			item_length = next_slash - cert_subject ;
                }
                else
                {
                        item_length = strlen ( cert_subject ) ;
                }
			
                if ( item_length > 39 )
                        item_length = 39 ;
			
                strncpy ( next_item , cert_subject , item_length ) ;
                next_item [item_length] = '\0' ;
			
                printf ( "%s" , next_item ) ;

                if ( next_slash != NULL )
                {
			strcpy ( cert_subject_temp , next_slash + 1 ) ;
			strcpy ( cert_subject , cert_subject_temp ) ;
		}
                else
                {
                        strcpy ( cert_subject , "" ) ;
		}
                                                                                                		
		printf ( "%*c" , 40 - (int) strlen ( next_item ) , ' ' ) ;
		
		/* issuer */

		next_slash = strstr ( cert_issuer , "/" ) ;
		
		if ( next_slash != NULL )
		{
			item_length = next_slash - cert_issuer ;
                }
                else
                {
                        item_length = strlen ( cert_issuer ) ;
                }
			
                if ( item_length > 39 )
                        item_length = 39 ;
			
                strncpy ( next_item , cert_issuer , item_length ) ;
                next_item [item_length] = '\0' ;
			
                printf ( "%s" , next_item ) ;
                
                if ( next_slash != NULL )
                {	
			strcpy ( cert_issuer_temp , next_slash + 1 ) ;
			strcpy ( cert_issuer , cert_issuer_temp ) ;
		}
                else
                {
                        strcpy ( cert_issuer , "" ) ;
		}
		
		printf ( "\n" ) ;
	}
	
	/* cleanup */
	
        free ( cert_subject ) ;
        free ( cert_subject_temp ) ;
	free ( cert_issuer ) ;
	free ( cert_issuer_temp ) ;
	
	/* fingerprint */
	
	if ( X509_digest ( server_cert , EVP_sha256() , fingerprint , &length ) 
	     != 0 )
	{
		printf ( "\nSHA256 Fingerprint: " ) ;
		for ( counter = 0 ; counter < length ; counter ++ )
		{
			printf ( "%02x" , fingerprint [counter] ) ;
			if ( counter != length -1 )
				printf ( ":" ) ;
		}
		printf ( "\n" ) ;
	}
	
	/* dates */

	bio_out = BIO_new_fp ( stdout , BIO_NOCLOSE ) ;
	if ( bio_out != NULL )
	{
		printf ( "Valid from " ) ;
		ASN1_TIME_print ( bio_out , 
		                  X509_get_notBefore ( server_cert ) ) ;
		printf ( " to " ) ;
		ASN1_TIME_print ( bio_out , 
		                  X509_get_notAfter ( server_cert ) ) ;
		printf ( "\n" ) ;
		BIO_free ( bio_out ) ;
	}

	/* verify certificate */
	
	verify_result = SSL_get_verify_result ( this_ssl_connection ) ;

	printf ( "\nCertificate validation: %s\n" ,
	            X509_verify_cert_error_string ( verify_result ) ) ;

	/* cleanup */

	X509_free ( server_cert ) ;

	/* return */

	return SHOWMAIL_SOCKET_SUCCESS ;
}
