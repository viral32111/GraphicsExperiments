// My custom window class
#include "MyWindow.h"

LRESULT CALLBACK MyWindow::windowProcedure( HWND windowHandle, UINT messageCode, WPARAM wParam, LPARAM lParam ) {

	// Run a custom handler depending on the received code
	switch ( messageCode ) {

	}

	// Run default action for the message if we do not have a custom handler for it
	return DefWindowProc( windowHandle, messageCode, wParam, lParam );

}

void MyWindow::onWindowDestroy( HWND windowHandle ) {

}

void MyWindow::onWindowResize( HWND windowHandle, UINT type, UINT width, UINT height ) {

}

void MyWindow::onWindowPaint( HWND windowHandle ) {

}
