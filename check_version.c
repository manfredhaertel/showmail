/* function check_version - check match of binary and library */

#include "showmail.h"

void check_version ( char *binary_version )
{
        char library_version [10] ;
        int binary_major ;
        int binary_minor ;
        int library_major ;
        int library_minor ;
        
        sprintf ( library_version , PACKAGE_VERSION ) ;
        
        binary_major = atoi ( binary_version ) ;
        binary_minor = atoi ( strchr ( binary_version , '.' ) + 1 ) ;

        library_major = atoi ( library_version ) ;
        library_minor = atoi ( strchr ( library_version , '.' ) + 1 ) ;

        /* versions match, return without a message */
        
        if ( ( binary_major == library_major ) && ( binary_minor == library_minor ) )
                return ;
                
        /* library is newer than the binary. this will probably work,
           but give a warning anyway */
           
        if ( ( library_major > binary_major ) ||
             ( ( library_major == binary_major ) && ( library_minor > binary_minor ) ) )
        {
                printf ( "WARNING: library is newer than binary - please check your installation\n" ) ;
                printf ( "binary: v%s, library: v%s\n\n" , binary_version , library_version ) ;
                return ;
        }
        
        /* otherwise the binary version is newer than the library version. 
           this is really bad, so give an error message */
           
        printf ( "ERROR: binary is newer than library - please check your installation\n" ) ;
        printf ( "binary: v%s, library: v%s\n" , binary_version , library_version ) ;
        printf ( "since this is bad, we refuse to run\n" ) ;
        
        exit ( 1 ) ;
}
