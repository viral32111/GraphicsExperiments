// Windows API
#include <Windows.h>

// Visual Styles
#include <CommCtrl.h>

// My custom window class
#include "MyWindow.h"

// Console functions
#include "Console.h"

// Thread experiments
#include "Thread.h"

// Prototypes for functions later on in this file
void initializeCommonControls();

/*
 1st parameter is a handle to the instance of the application/executable when loaded in memory.
 2nd parameter is no longer used, so it is always zero.
 3rd parameter is the command-line arguments used to launch the executable, as a Unicode (wide character) string.
 4th parameter is a flag for if the window should be minimized, maximized, or shown normally.

 https://docs.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point

 The _In_ and _In_opt_ are required SAL annotations.
*/
int WINAPI wWinMain( _In_ HINSTANCE applicationHandle, _In_opt_ HINSTANCE _, _In_ PWSTR commandLineParameters, _In_ int showWindowFlags ) {

	// Create a console window
	consoleCreate( "Created console window." );

	// Initialize the common control classes before creating UI
	initializeCommonControls();

	// Options for my class
	LPCWSTR windowClassName = L"My Window Class";
	LPCWSTR windowTitle = L"My Window";
	UINT windowWidth = 800;
	UINT windowHeight = 600;

	// Create an instance of my custom window class using the options above
	MyWindow myWindow( windowClassName, windowTitle, windowWidth, windowHeight );

	// Setup & register the window class
	myWindow.setupWindowClass( applicationHandle );

	// Create & show the top-level window
	myWindow.createMainWindow( applicationHandle, showWindowFlags );

	// Setup the Direct2D resources
	myWindow.setupDirect2D();

	// Create thread
	threadCreate();

	// Start pulling window messages, this will block until a quit message is received
	myWindow.pullWindowMessages();

	// Release all the Direct2D resources (this should have already been done, but do it again just in case)
	myWindow.releaseDirect2D();

	// Stop thread
	//threadStop();

	// Close the console window
	consoleClose( "Closing console window..." );

	// Finish with a success status code
	return 0; // This is not used by Windows, but it may be useful for other applications that execute us

}

// Initializes the common control classes, required by Visual Styles 6
// https://docs.microsoft.com/en-us/windows/win32/controls/cookbook-overview
void initializeCommonControls() {

	// Structure that defines what control classes should be loaded
	// https://docs.microsoft.com/en-us/windows/win32/api/commctrl/ns-commctrl-initcommoncontrolsex
	INITCOMMONCONTROLSEX commonControls { 0 };
	commonControls.dwSize = sizeof( INITCOMMONCONTROLSEX );
	commonControls.dwICC = ICC_STANDARD_CLASSES; // Usual User32 control classes

	// Load those common controls
	if ( InitCommonControlsEx( &commonControls ) == FALSE ) {
		consoleError( "Failed to initialize the common controls library!" );
		ExitProcess( 1 );
		return;
	}

	// Display message to console
	consoleOutput( "Initialized the common control classes." );

}
