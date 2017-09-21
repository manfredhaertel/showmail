/* function scan_header - scan a mail header for a certain line */

#include "showmail.h"

/* utility function for base64 decoding */

int b64value ( char thischar )
{
	if ( ( thischar >= 'A' ) && ( thischar <= 'Z' ) )
	{
		return thischar - 'A' ;
	}

	if ( ( thischar >= 'a' ) && ( thischar <= 'z' ) )
	{
		return thischar - 'a' + 26 ;
	}

	if ( ( thischar >= '0' ) && ( thischar <= '9' ) )
	{
		return thischar - '0' + 52 ;
	}
	
	if ( thischar == '+' )
	{
		return 62 ;
	}
	
	if ( thischar == '/' )
	{
		return 63 ;
	}

	return 0 ;
}

void scan_header ( char *header , char *item , char *headerline )
{	
	char searchstring [MAXSTRING] ;
	char *itempointer ;
	char itemstring [MAXBUF] ;
	char decheaderline [MAXBUF] ;
	int offset ;
	int decoffset ;

	/* get e.g. the "From:" line, starting with cr,lf,"From:" and ending
           with a carriage return */
           
        sprintf ( searchstring , "\r\n%s:" , item ) ;   

	itempointer = strstr ( header , searchstring ) ;
	
	if ( itempointer != NULL )
	{
		strcpy ( itemstring , itempointer + 2 ) ;
		if ( strstr ( itemstring , "\r" ) != NULL )
			* strstr ( itemstring , "\r" ) = '\0' ;
		strcpy ( headerline , itemstring ) ;
	}
	else
	{
		/* there was no headerline for the item */

		strcpy ( headerline , "" ) ;
	}

	/* is subject base64? */
	
	if ( ( strcmp ( item , "Subject" ) == 0 ) &&
	     ( strstr ( headerline , "=?" ) != NULL ) &&
	     ( strstr ( headerline , "?B?" ) != NULL ) &&
	     ( strstr ( headerline , "?=" ) != NULL ) &&
	     ( strstr ( headerline , "?B?" ) - 
	       strstr ( headerline , "=?" ) > 0 ) &&
	     ( strstr ( headerline , "?=" ) - 
	       strstr ( headerline , "?B?" ) > 0 ) )
	{
		decoffset = 0 ;
		
		for ( offset = strstr ( headerline , "?B?" ) + 3 - headerline ;
		      offset < strlen ( headerline ) - 2 ; 
		      offset += 4 )
		{
			decheaderline[decoffset] = ( b64value ( headerline[offset] ) << 2 ) + ( b64value ( headerline[offset+1] ) >> 4 ) ;
			decheaderline[decoffset+1] = ( ( b64value ( headerline[offset+1] ) & 15 ) << 4 ) + ( b64value ( headerline[offset+2] ) >> 2 ) ;
			decheaderline[decoffset+2] = ( ( b64value ( headerline[offset+2] ) & 3 ) << 6 ) + b64value ( headerline[offset+3] ) ;
			decoffset += 3 ;
		}

		decheaderline[decoffset] = '\0' ;

		strcpy ( strstr ( headerline , "=?" ) , decheaderline ) ;
	}
}
