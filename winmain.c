/* winmain.c - main program for winshowmail */

/* includes */

#include <windows.h>
#include <commdlg.h>
#include "showmail.h"

/* defines */

#define MAXLINES 1000

/* global variables */

typedef struct
{
	HANDLE hInstance;
	HWND hMainWnd;
	HMENU hMainMenu;
} GLOBALS ;

GLOBALS Globals;

char server [MAXSTRING] ;
char protocol [MAXSTRING] ;
char user [MAXSTRING] ;
char password [MAXSTRING] ;
char feature [MAXSTRING] ;
int lastline ;
char output [MAXLINES][MAXSTRING] ;
HWND output_window ;
int scroll_position ;
int virtual_height ;
int physical_height_pixel ;
int text_height ;

/* function get_info - get informations from pop server */

void get_info ( void ) 
{
        SECURITY_ATTRIBUTES saAttr ;
        HANDLE hChildStd_IN_Rd = NULL ;
        HANDLE hChildStd_IN_Wr = NULL ;
        HANDLE hChildStd_OUT_Rd = NULL ;
        HANDLE hChildStd_OUT_Wr = NULL ;
        CHAR szCmdline [MAXSTRING] ;
        PROCESS_INFORMATION piProcInfo ; 
        STARTUPINFO siStartInfo ;
        BOOL bSuccess = FALSE ;
        DWORD dwRead , dwWritten ; 
        CHAR chBuf [MAXBUF] ;
        int counter ;
        int offset ;
                
        /* before the query is finished, we have no lines */

        lastline = -1 ; 
        
        /* get the infos from the console application */

        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
        saAttr.bInheritHandle = TRUE; 
        saAttr.lpSecurityDescriptor = NULL; 
 
        CreatePipe ( &hChildStd_OUT_Rd , &hChildStd_OUT_Wr , &saAttr , 0 ) ;
        SetHandleInformation ( hChildStd_OUT_Rd , HANDLE_FLAG_INHERIT , 0 ) ;

        CreatePipe ( &hChildStd_IN_Rd , &hChildStd_IN_Wr , &saAttr , 0 ) ; 
        SetHandleInformation ( hChildStd_IN_Wr , HANDLE_FLAG_INHERIT , 0 ) ;

        ZeroMemory ( &piProcInfo , sizeof ( PROCESS_INFORMATION ) ) ;
        
        ZeroMemory ( &siStartInfo , sizeof ( STARTUPINFO ) ) ;
        siStartInfo.cb = sizeof ( STARTUPINFO ) ;
        siStartInfo.hStdError = hChildStd_OUT_Wr ;
        siStartInfo.hStdOutput = hChildStd_OUT_Wr ;
        siStartInfo.hStdInput = hChildStd_IN_Rd ;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES ;
        
        sprintf ( szCmdline , "showmail %s %s %s - %s" , 
                  server , protocol , user , feature ) ;
        
        bSuccess = CreateProcess ( NULL , szCmdline , NULL , NULL , TRUE , 0 , NULL , NULL ,
                                   &siStartInfo , &piProcInfo ) ;
        if ( ! bSuccess )
        {
                MessageBox ( (HWND) 0 , (LPSTR) "could not start showmail.exe" ,
                             (LPSTR) "Error" , MB_OK ) ;                
                return ;
        }

        /* close unneeded handles */

        CloseHandle ( hChildStd_OUT_Wr ) ;
        CloseHandle ( hChildStd_IN_Rd ) ;
        CloseHandle ( piProcInfo.hProcess ) ;
        CloseHandle ( piProcInfo.hThread ) ;
        
        /* send password */

        sprintf ( chBuf , "%s\r\n" , password ) ;
        bSuccess = WriteFile ( hChildStd_IN_Wr , chBuf , strlen ( chBuf ) , &dwWritten , NULL ) ;
        if ( ! bSuccess )
        {
                MessageBox ( (HWND) 0 , (LPSTR) "could not send password to showmail.exe" ,
                             (LPSTR) "Error" , MB_OK ) ;
                return ;
        }
        
        /* read response */
        
        offset = 0 ;
        lastline = 0 ;
        do
        {
                bSuccess = ReadFile ( hChildStd_OUT_Rd , chBuf , MAXBUF , &dwRead , NULL ) ;
                if ( bSuccess && ( dwRead > 0 ) )
                {
                        for ( counter = 0 ; counter < dwRead ; counter ++ )
                        {
                                if ( chBuf[counter] != '\n' )
                                {
                                        output[lastline][offset] = chBuf[counter] ;
                                        offset ++ ;         
                                }
                                else
                                {
                                        lastline ++ ;
                                        offset = 0 ;
                                }
                        }
                }
        }
        while ( bSuccess && ( dwRead > 0 ) ) ;
        lastline -- ;
        
        /* close handles */
        
        CloseHandle ( hChildStd_IN_Wr ) ;
        CloseHandle ( hChildStd_OUT_Rd ) ;
        
        /* init scrolling information */

   	scroll_position = 0 ;

        /* update main window */

	InvalidateRgn ( output_window , 0 , TRUE ) ;
	UpdateWindow ( output_window ) ;                
}

/* function Dlg_Proc - callback for the dialogs */

LRESULT CALLBACK DlgProc ( HWND hWnd , UINT msg , WPARAM wParam , 
                           LPARAM lParam )
{
        HWND server_control ;
	HWND protocol_control ;
	HWND user_control ;
	HWND password_control ;

    	switch(msg)
    	{
		case WM_INITDIALOG:
		
			/* if we already know server etc. we suggest the
			   user to reuse them */
	
	    		server_control = GetDlgItem ( hWnd , 201 ) ;
	    		SetWindowText ( server_control , server ) ;
	
	    		protocol_control = GetDlgItem ( hWnd , 203 ) ;
	    		SetWindowText ( protocol_control , protocol ) ;
	
	    		user_control = GetDlgItem ( hWnd , 205 ) ;
	    		SetWindowText ( user_control , user ) ;
	
	    		password_control = GetDlgItem ( hWnd , 207 ) ;
	    		SetWindowText ( password_control , password ) ;
	
	    		break;
	    
		case WM_COMMAND:
	    		switch (wParam)
	    		{
				case 210:
					/* OK pressed, get input */
					
					server_control = 
		    				GetDlgItem ( hWnd , 201 ) ;
		    			GetWindowText ( server_control , 
		    					server , 
		        				sizeof ( server ) ) ;

		    			protocol_control = 
		    				GetDlgItem ( hWnd , 203 ) ;
		    			GetWindowText ( protocol_control , 
		    					protocol , 
		                    			sizeof ( protocol ) ) ;

		    			user_control = 
		    				GetDlgItem ( hWnd , 205 ) ;
		    			GetWindowText ( user_control , user , 
		                    			sizeof ( user ) ) ;

		    			password_control = 
		    				GetDlgItem ( hWnd , 207 ) ;
		    			GetWindowText ( password_control , 
		    					password , 
		                    			sizeof ( password ) ) ;
		                    			
		                    	/* query the server after we have all
		                    	   informations */

		    			get_info () ;

				case 211:
					/* cancel pressed, only end dialog */
				
		    			EndDialog ( hWnd , 100 ) ;

		    			return TRUE ;
	    		}
    	}
    	
	return FALSE ;
}

/* function WndProc - callback for window management */

LRESULT CALLBACK WndProc ( HWND wnd , UINT msg , WPARAM w , LPARAM l )
{
	HDC dc ;
	PAINTSTRUCT ps ;
    	TEXTMETRIC tm ;
    	int counter ;
    	char info_string [MAXSTRING] ;
    	char status_string [MAXSTRING] ;
    	int old_scroll_position ;
    	int first ;
    	int last ;
    
    	switch ( msg )
    	{
    		case WM_COMMAND:
			switch ( w )
			{
				case 100:
					/* input dialog box */
				
					DialogBox ( Globals.hInstance ,
						    "SHOWMAIL" , wnd ,
						    (DLGPROC) DlgProc ) ;
					return 0 ;
					
				case 101:
					/* exit button */
					
					PostQuitMessage ( 0 ) ;
					return 0 ;
					
				case 110:
					/* about button */
					
					sprintf ( info_string , 
				  	         "%s V%s by Manfred Haertel" ,
				  		  PACKAGE_NAME ,
				  		  PACKAGE_VERSION ) ;
				  		  
					MessageBox ( (HWND) 0 , 
					             (LPSTR) info_string ,
			             		     (LPSTR) "About SHOWMAIL" , 
			             		 MB_OK | MB_ICONINFORMATION ) ;
					return 0 ;
					
				default:
					return DefWindowProc ( wnd , msg , 
							       w , l ) ;
			}
			
    		case WM_DESTROY:
    			/* (external) kill of window */
    		
			PostQuitMessage ( 0 ) ;
			break ;
	
		case WM_PAINT:
			/* do the actual output */

			output_window = wnd ;
			
			/* get informations about text size */

    			dc = BeginPaint ( wnd , &ps ) ;
    			GetTextMetrics ( dc , &tm ) ;
    	
    			text_height = tm.tmHeight ;
    	
    			if ( lastline < 0 )
    			{
    				/* no query yet, status unknown */
    				
    				strcpy ( status_string , 
    		                         "Unknown number of messages" ) ;
    		
    				TextOut ( dc , 5 , 0 , status_string , 
    		          		  strlen ( status_string ) ) ;
    			}
    			else
			{
				/* otherwise output information */
				
				/* determine first and last line to display */
				
				first = scroll_position ;
				last  = scroll_position + 
				  ( physical_height_pixel / text_height ) + 1 ;

				/* avoid off limits */
		
				if ( first < 0 )
					first = 0 ;
				if ( last > lastline )
					last = lastline ;
					
				/* output */
	
				for ( counter = first ; counter <= last ; 
				      counter ++ )
				{
    					TextOut ( dc , 5 , 
    					    ( counter - first ) * text_height , 
    			     			  output [counter] , 
    			     		        strlen ( output [counter] ) ) ;
    				}
    			}

    			EndPaint ( wnd , &ps ) ;
    	
			virtual_height = lastline + 1 ;
			
			/* set scroll bar informations */

    			SetScrollRange ( output_window , SB_VERT , 0 , 
    	             virtual_height - ( physical_height_pixel / text_height ) , 
    	    				 TRUE ) ;
    	    				 
   			SetScrollPos ( output_window , SB_VERT , 
   				       scroll_position , TRUE ) ;

    			break ;
    	
    		case WM_SIZE:
    		
    			/* handle resize */
    
			/* get physical size of window */

    			physical_height_pixel = HIWORD ( l ) ;
    	
    			/* during the first size we do not know the 
    			   text_height, so exit */
    	
    			if ( text_height == 0 )
    				break ;
    
			/* reset scroll parameters due to resize */

    			SetScrollRange ( wnd , SB_VERT , 0 , 
    	             virtual_height - ( physical_height_pixel / text_height ) , 
    	    				 TRUE ) ;
    	    				 
    	    		/* if the window can display everything now
    	    		   because it is large enough, do this */

			if ( ( physical_height_pixel / text_height ) >
			     virtual_height )
			{
				scroll_position = 0 ;
				SetScrollPos ( wnd ,  SB_VERT , 
					       scroll_position , TRUE ) ;
			}

    			break ;
    	
    		case WM_VSCROLL:
    		
    			/* the user used the scroll bar */
    
    			old_scroll_position = scroll_position ;

    			switch ( LOWORD ( w ) )
    			{
    				case SB_PAGEUP:
    					scroll_position -= 
    				     ( physical_height_pixel / text_height ) ;
					break ;
    		
    				case SB_LINEUP:
    					scroll_position -- ;
    					break ;
    			
    				case SB_PAGEDOWN:
    					scroll_position += 
    				     ( physical_height_pixel / text_height ) ;
					break ;
    		
    				case SB_LINEDOWN:
    					scroll_position ++ ;
    					break ;
    			
    				case SB_THUMBTRACK:
    					scroll_position = HIWORD ( w ) ;
    					break ;
    			}
    	
    			/* avoid scrolling too far */
    	
    			if ( scroll_position > 
    	            ( virtual_height - physical_height_pixel / text_height ) )
    				scroll_position = virtual_height 
    			    	   - ( physical_height_pixel / text_height ) ;
    	
    			if ( scroll_position < 0 )
    				scroll_position = 0 ;
    				
			/* if actually not scrolled: nothing to do! */
			
			if ( scroll_position == old_scroll_position )
				break ;
			
			/* reset scrolling parameters and redraw */
    		
    			SetScrollPos ( wnd , SB_VERT , scroll_position , 
    				       TRUE ) ;
    			ScrollWindow ( wnd , 0 , 
    		      ( old_scroll_position - scroll_position ) * text_height ,
    		                       NULL , NULL ) ;
    			UpdateWindow ( wnd ) ;
    	
    			break ;

    		default:
			return DefWindowProc (wnd, msg, w, l);
    	}
    
    	return 0l;
}

/* function WinMain - main function for windows */

int APIENTRY WinMain ( HINSTANCE inst ,HINSTANCE prev , LPSTR cmdline , int show )
{
	MSG msg ;
    	WNDCLASS class ;
    	char className[] = "class" ;
    	char winName[] = "SHOWMAIL" ;
    	
    	/* we do not yet know something about the server etc. */

	strcpy ( server , "" ) ;
	strcpy ( protocol , "" ) ;
	strcpy ( user , "" ) ;
	strcpy ( password , "" ) ;
	
	/* get parameters, if there are any */

	get_parameter ( -1 , NULL , server , protocol , user , 
	                password , feature ) ;

	/* we have no lines for output */
	
	lastline = -1 ;
	
	/* windows init stuff */

    	Globals.hInstance = inst ;
    	if ( ! prev )
    	{
		class.style = CS_HREDRAW | CS_VREDRAW ;
		class.lpfnWndProc = WndProc ;
		class.cbClsExtra = 0 ;
		class.cbWndExtra = 0 ;
		class.hInstance = inst ;
		class.hIcon = LoadIcon ( 0 , IDI_ASTERISK ) ;
		class.hCursor = LoadCursor  ( 0 , IDC_ARROW ) ;
		class.hbrBackground = GetStockObject ( WHITE_BRUSH ) ;
		class.lpszMenuName = 0 ;
		class.lpszClassName = className ;
    	}
    	
    	if ( ! RegisterClass ( &class ) )
		return FALSE;

    	Globals.hMainWnd = CreateWindow ( className , winName , 
    	                                  WS_OVERLAPPEDWINDOW ,
					  CW_USEDEFAULT , 0 , CW_USEDEFAULT , 
					  0 , 0 , LoadMenu ( inst , "MAIN" ) , 
					  inst , 0 ) ;
    	ShowWindow ( Globals.hMainWnd , show ) ;
    	UpdateWindow ( Globals.hMainWnd ) ;

	/* show dialog box at the very start */

	DialogBox ( Globals.hInstance , "SHOWMAIL" , Globals.hMainWnd ,
		    (DLGPROC) DlgProc ) ;

    	while ( GetMessage ( &msg , 0 , 0 , 0 ) )
    	{
		TranslateMessage ( &msg ) ;
		DispatchMessage ( &msg ) ;
    	}
    
    	return 0;
}
