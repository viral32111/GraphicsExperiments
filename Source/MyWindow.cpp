// My custom window class
#include "MyWindow.h"

// Set class properties when this class is created
MyWindow::MyWindow( LPCWSTR windowClassName, LPCWSTR windowTitle, UINT windowWidth, UINT windowHeight ) :
	WINDOW_CLASS_NAME( windowClassName ),
	WINDOW_TITLE( windowTitle ),
	WINDOW_WIDTH( windowWidth ),
	WINDOW_HEIGHT( windowHeight ) {

}

// Release Direct2D resources when this class is destroyed
MyWindow::~MyWindow() {
	this->releaseDirect2D();
}
