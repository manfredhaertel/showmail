/* function get_parameter - get parameters from command line, resource file
   and keyboard */

#include "showmail.h"

void get_parameter ( int argc , char **argv , 
                     char *server , char *protocol , char *user , char *password , char *feature )
{
	char server_command [MAXSTRING] ;
	char protocol_command [MAXSTRING] ;
	char user_command [MAXSTRING] ;
	char password_command [MAXSTRING] ;
	char feature_command [MAXSTRING] ;
	char server_rc [MAXSTRING] ;
	char protocol_rc [MAXSTRING] ;
	char user_rc [MAXSTRING] ;
	char password_rc [MAXSTRING] ;
	char feature_rc [MAXSTRING] ;
	FILE *rcfile ;
	char rcfilename [MAXSTRING] ;
	int console_version ;
	struct stat homestat ;
#ifdef HAVE_TERMIOS_H
	struct termios withecho ;
	struct termios noecho ;
#endif
#ifdef HAVE_GETCH
	int pwoffset ;
#endif
	
	/* argc = -1 means: windows version, don't ask on the console */
	
	if ( argc != -1 )
		console_version = 1 ;
	else
		console_version = 0 ;
	
	/* get command line parameters - marking nonexisting parameters
	   as unspecified */
	
	if ( argc >= 2 )
		strcpy ( server_command , argv [1] ) ;
	else
		strcpy ( server_command , "-" ) ;
		
	if ( argc >= 3 )
		strcpy ( protocol_command , argv [2] ) ;
	else
		strcpy ( protocol_command , "-" ) ;
		
	if ( argc >= 4 )
		strcpy ( user_command , argv [3] ) ;
	else
		strcpy ( user_command , "-" ) ;
		
	if ( argc >= 5 )
		strcpy ( password_command , argv [4] ) ;
	else
		strcpy ( password_command , "-" ) ;
		
	if ( argc >= 6 )
		strcpy ( feature_command , argv [5] ) ;
	else
		strcpy ( feature_command , "-" ) ;
		
	/* is there a home dir? */
	
	if ( ( getenv ( "HOME" ) != NULL ) &&
	     ( strlen ( getenv ( "HOME" ) ) > 0 ) &&
	     ( strlen ( getenv ( "HOME" ) ) < MAXSTRING - 1 ) )
	{
		sprintf ( homedir , "%s" , getenv ( "HOME" ) ) ;
	}
	else if ( ( getenv ( "HOMEDRIVE" ) != NULL ) &&
		  ( getenv ( "HOMEPATH" ) != NULL ) &&
		  ( strlen ( getenv ( "HOMEDRIVE" ) ) +
		    strlen ( getenv ( "HOMEPATH" ) ) > 0 ) &&
		  ( strlen ( getenv ( "HOMEDRIVE" ) ) +
		    strlen ( getenv ( "HOMEPATH" ) ) < MAXSTRING - 1 ) )
	{
		sprintf ( homedir , "%s%s" , getenv ( "HOMEDRIVE" ) ,
					     getenv ( "HOMEPATH" ) ) ;
	}
	else
	{
		strcpy ( homedir , "" ) ;
	}
	
	if ( strlen ( homedir ) > 0 )
	{
		if ( stat ( homedir , &homestat ) != 0 )
		{
			strcpy ( homedir , "" ) ;
		}
	}

	/* is there a resource file? */

	if ( ( strlen ( homedir ) > 0 ) &&
	     ( strlen ( homedir ) + strlen ( "/.showmailrc" ) < MAXSTRING - 1 ) )
	{
		sprintf ( rcfilename , "%s/.showmailrc" , homedir ) ;

		rcfile = fopen ( rcfilename , "r" ) ;
	}
	else
	{
		rcfile = NULL ;
	}
	
	if ( rcfile != NULL )
	{
		/* read it, ignoring lines starting with a hash */
		
		do
			fgets ( server_rc , MAXSTRING-2 , rcfile ) ;
		while ( server_rc [0] == '#' ) ;
		
		do
			fgets ( protocol_rc , MAXSTRING-2 , rcfile ) ;
		while ( protocol_rc [0] == '#' ) ;
		
		do
			fgets ( user_rc , MAXSTRING-2 , rcfile ) ;
		while ( user_rc [0] == '#' ) ;
		
		do
			fgets ( password_rc , MAXSTRING-2 , rcfile ) ;
		while ( password_rc [0] == '#' ) ;
		
		do
			fgets ( feature_rc , MAXSTRING-2 , rcfile ) ;
		while ( feature_rc [0] == '#' ) ;
		
		fclose ( rcfile ) ;
		
		/* chop lfs at the end */
		
		server_rc [strlen(server_rc)-1] = '\0' ;
		protocol_rc [strlen(protocol_rc)-1] = '\0' ;
		user_rc [strlen(user_rc)-1] = '\0' ;
		password_rc [strlen(password_rc)-1] = '\0' ;
		feature_rc [strlen(feature_rc)-1] = '\0' ;
	}
	else
	{
		/* if the file does not exist, the parameters are unspecified,
		   that is, dashes */
		   
		strcpy ( server_rc , "-" ) ;
		strcpy ( protocol_rc , "-" ) ;
		strcpy ( user_rc , "-" ) ;
		strcpy ( password_rc , "-" ) ;
		strcpy ( feature_rc , "-" ) ;
	}

	/* interpret parameters:
	   - if command line parameter is specified, it is used, even if
	     there is something specified in the resource file
	   - otherwise, if resource file parameter is specified, it is used
	   - otherwise, ask for the parameter
	 */

	if ( strcmp ( server_command , "-" ) != 0 )
		strcpy ( server , server_command ) ;
	else if ( strcmp ( server_rc , "-" ) != 0 )
		strcpy ( server , server_rc ) ;
	else if ( console_version )
	{
		printf ( "Server: " ) ;
		fgets ( server , MAXSTRING-1 , stdin ) ;
		server [strlen(server)-1] = '\0' ;
	}
	
	if ( strcmp ( protocol_command , "-" ) != 0 )
		strcpy ( protocol , protocol_command ) ;
	else if ( strcmp ( protocol_rc , "-" ) != 0 )
		strcpy ( protocol , protocol_rc ) ;
	else if ( console_version )
	{
		printf ( "Protocol: " ) ;
		fgets ( protocol , MAXSTRING-1 , stdin ) ;
		protocol [strlen(protocol)-1] = '\0' ;
	}
	
	if ( strcmp ( user_command , "-" ) != 0 )
		strcpy ( user , user_command ) ;
	else if ( strcmp ( user_rc , "-" ) != 0 )
		strcpy ( user , user_rc ) ;
	else if ( console_version )
	{
		printf ( "User: " ) ;
		fgets ( user , MAXSTRING-1 , stdin ) ;
		user [strlen(user)-1] = '\0' ;
	}
	
	if ( strcmp ( password_command , "-" ) != 0 )
		strcpy ( password , password_command ) ;
	else if ( strcmp ( password_rc , "-" ) != 0 )
		strcpy ( password , password_rc ) ;
	else if ( console_version )
	{
		/* don't echo the password */

		printf ( "Password: " ) ;

#ifdef HAVE_TERMIOS_H
		/* Linux and possibly many other OS */
	
		/* system ( "stty -echo" ) ; */
		tcgetattr ( 0 , &withecho ) ;
		noecho = withecho ;
		noecho.c_lflag &= ~ECHO ;
		tcsetattr ( 0 , TCSANOW , &noecho ) ;	
		
		fgets ( password , MAXSTRING-1 , stdin ) ;
		
		/* system ( "stty echo" ) ; */
		tcsetattr ( 0 , TCSAFLUSH , &withecho ) ;
#else
#ifdef HAVE_GETCH
		/* Windows */

		if ( isatty ( STDIN_FILENO ) )
		{
		        /* if stdin is a tty, read password hidden */
		        
		        pwoffset = 0 ;
		        do
		        {
			        password[pwoffset] = getch () ;
			        pwoffset ++ ;
                        }
                        while ( ( password[pwoffset-1] != '\r' ) && 
                        	( pwoffset < MAXSTRING - 1 ) ) ;
	        	password[pwoffset] = '\0' ;
                }
                else
                {
                        /* if stdin is not a tty, read password normally */
                        
                        fgets ( password , MAXSTRING-1 , stdin ) ;
                }
#else
#error No password routine implemented for your environment
#endif
#endif
		password [strlen(password)-1] = '\0' ;
		printf ( "\n" ) ;
	}
	
	if ( strcmp ( feature_command , "-" ) != 0 )
		strcpy ( feature , feature_command ) ;
	else if ( strcmp ( feature_rc , "-" ) != 0 )
		strcpy ( feature , feature_rc ) ;
	else if ( console_version )
	{
		printf ( "Feature: " ) ;
		fgets ( feature , MAXSTRING-1 , stdin ) ;
		feature [strlen(feature)-1] = '\0' ;
	}

	/* if debugging is turned on: display parameters */
	
	if ( debug )
	{
		printf ( "Server = %s\n" , server ) ;
		printf ( "Protocol = %s\n" , protocol ) ;
		printf ( "User = %s\n" , user ) ;
		printf ( "Password = %s\n" , password ) ;
		printf ( "Feature = %s\n" , feature ) ;
	}
}
