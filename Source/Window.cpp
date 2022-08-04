// My custom window class
#include "MyWindow.h"

// Creates and registers the window class
void MyWindow::setupWindowClass( HINSTANCE applicationInstance ) {

	// Create an extended window class structure
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof( WNDCLASSEX );

	// Populate that structure with relevant data
	windowClass.lpfnWndProc = MyWindow::windowProcedure; // The method that will receive window messages
	windowClass.lpszClassName = this->WINDOW_CLASS_NAME; // Identifier of this window class
	windowClass.hInstance = applicationInstance; // Handle to the application instance, from the main entry-point
	windowClass.hCursor = LoadCursor( NULL, IDC_ARROW ); // Default to the normal cursor, see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadcursora
	windowClass.hIcon = NULL; // Use the default application icon
	windowClass.hIconSm = NULL; // No small icon
	windowClass.style = CS_HREDRAW | CS_VREDRAW; // Redraw entire window if width or height of the client area changes, see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-class-styles

	// Register the extended window class using that structure
	if ( RegisterClassExW( &windowClass ) == NULL ) {
		DWORD latestErrorCode = GetLastError();
		std::cerr << "Failed to register the window class: " << latestErrorCode << std::endl;
		ExitProcess( 1 );
	}

}

// Creates the top-level window
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window
void MyWindow::createMainWindow( HINSTANCE applicationInstance, int showWindowFlags ) {

	// Create an extended styles window
	this->windowHandle = CreateWindowExW(
		WS_EX_DLGMODALFRAME, // Hide the icon, see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
		this->WINDOW_CLASS_NAME, // The name of the window class
		this->WINDOW_TITLE, // The text displayed as the window title
		WS_OVERLAPPEDWINDOW, // Use the default flags for a window (title bar, border, minimize & maximize buttons, etc.)
		CW_USEDEFAULT, CW_USEDEFAULT, // Position the window wherever the system wants it
		this->WINDOW_WIDTH, this->WINDOW_HEIGHT, // The size of the window, could be CW_USEDEFAULT to use default values
		NULL, // No parent window as this is the top-level window
		NULL, // No menu for this window
		applicationInstance, // Handle to the instance of this application
		this // Extra data is a reference to this class so that message handlers can access it, used in the WM_CREATE message
	);

	// Do not continue if there was an issue creating the window
	if ( windowHandle == NULL ) {
		DWORD latestErrorCode = GetLastError();
		std::cerr << "Failed to create the window: " << latestErrorCode << std::endl;
		ExitProcess( 1 );
	}

	// Show the newly created window using the system's window show flag from the main entry-point
	ShowWindow( windowHandle, showWindowFlags );

}

// Constantly pulls window messages from the message queue until a quit message is pulled
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/window-messages
void MyWindow::pullWindowMessages() {

	// Will hold a window message that gets pulled from the message queue
	MSG windowMessage = { 0 };

	// The message loop runs when a message is pulled from the queue, and blocks when no messages are available (this does not freeze the window though)
	while ( GetMessageW( &windowMessage, NULL, 0, 0 ) > 0 ) {

		// Translate keystrokes into characters (must be called before DispatchMessage())
		TranslateMessage( &windowMessage );

		// Tell the operating system to call the window procedure for the target window of this message
		DispatchMessageW( &windowMessage );

	}

}
