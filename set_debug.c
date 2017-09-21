/* function set_debug - sets debug value. this is needed for cygwin, 
   in windows an executable and a library can not share a global variable,
   so we have to call a function from the main program which sets the
   value in the dll - sigh */

#include "showmail.h"

void set_debug ( int debug_value )
{	
	debug = debug_value ;
}
