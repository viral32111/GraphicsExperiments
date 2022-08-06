// My custom window class
#include "MyWindow.h"

// Console functions
#include "Console.h"

// Receives and handles messages dispatched to our window, ideally should be done on another thread as another message cannot be received until this finishes processing the current one
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/writing-the-window-procedure
LRESULT CALLBACK MyWindow::windowProcedure( HWND windowHandle, UINT messageCode, WPARAM wParam, LPARAM lParam ) {

	// Run a custom handler depending on the received code
	switch ( messageCode ) {

		// Window create
		// https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
		case WM_CREATE: {

			// https://stackoverflow.com/a/20256795

			// Get the custom class reference provided as extra data when creating the window
			LPCREATESTRUCT windowData = ( LPCREATESTRUCT ) lParam;
			MyWindow *myWindow = ( MyWindow * ) windowData->lpCreateParams;

			// Do not continue if the class reference is invalid
			if ( myWindow == NULL ) {
				consoleError( "Extra window data as class reference is invalid!" );
				return 1; // Abort the creation of the window
			}

			// Clear the latest error
			SetLastError( 0 );

			// Store the class reference in the user-data of the window, so it can be retrieved within the handlers
			// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowlongptrw
			if ( SetWindowLongPtrW( windowHandle, GWLP_USERDATA, ( LONG_PTR ) myWindow ) == NULL && GetLastError() != 0 ) {
				consoleError( "Failed to store class reference in window userdata! (%lu)", GetLastError() );
				return 1; // Abort the creation of the window
			} else {
				consoleOutput( "Class reference stored in window userdata." );
				return 0; // We processed this
			}

			break;

		}

		// Window close (when X button is clicked, etc.)
		// https://docs.microsoft.com/en-gb/windows/win32/winmsg/wm-close
		case WM_CLOSE: {

			// Show a prompt requesting that the user confirm this action, if they confirm then destroy the window
			// https://docs.microsoft.com/en-us/windows/win32/dlgbox/using-dialog-boxes
			if ( MessageBox( windowHandle, L"Are you sure you want to exit the application?", L"Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES ) {
				consoleOutput( "User confirmed closure of window." );
				DestroyWindow( windowHandle );
			}

			consoleOutput( "User denied closure of window." );
			return 0; // We always process this, even if the user denies the prompt so that the window does not close due to the default handler

			break;

		}

		// Window resized
		// https://docs.microsoft.com/en-gb/windows/win32/winmsg/wm-size
		case WM_SIZE: {

			// Retrieve the reference to our custom class from the window user-data
			MyWindow *myWindow = ( MyWindow * ) GetWindowLongPtrW( windowHandle, GWLP_USERDATA );

			// Call the handler on the class, with the type, and low-order & high-order words as the new width & height
			if ( myWindow != NULL ) {
				myWindow->onWindowResize( windowHandle, ( UINT ) wParam, LOWORD( lParam ), HIWORD( lParam ) );
				return 0; // We processed this
			}

			break;

		}

		// Window destroyed (called after window closed)
		// https://docs.microsoft.com/en-gb/windows/win32/winmsg/wm-destroy
		case WM_DESTROY: {

			// Retrieve the reference to our custom class from the window user-data
			MyWindow *myWindow = ( MyWindow * ) GetWindowLongPtrW( windowHandle, GWLP_USERDATA );

			// Call the handler on the class
			if ( myWindow != NULL ) {
				myWindow->onWindowDestroy( windowHandle );
				return 0; // We processed this
			}

			break;

		}

		// Window client area needs painting (called on initial show, on resize, etc) 
		// https://docs.microsoft.com/en-us/windows/win32/gdi/wm-paint
		case WM_PAINT: {
			
			// Retrieve the reference to our custom class from the window user-data
			MyWindow *myWindow = ( MyWindow * ) GetWindowLongPtrW( windowHandle, GWLP_USERDATA );
			
			// Call the handler on the class
			if ( myWindow != NULL ) {
				myWindow->onWindowPaint( windowHandle );
				return 0; // We processed this
			}

			break;

		}

		// When the window size or position is about to change
		// https://docs.microsoft.com/en-gb/windows/win32/winmsg/wm-getminmaxinfo
		case WM_GETMINMAXINFO: {

			// Get the min-max-info structure from the parameter
			LPMINMAXINFO minMaxData = ( LPMINMAXINFO ) lParam;

			// Set the minimum width and height
			minMaxData->ptMinTrackSize.x = 400;
			minMaxData->ptMinTrackSize.y = 350;

			return 0; // We processed this

			break;
			
		}

	}

	// Run default action for the message if we do not have a custom handler for it
	return DefWindowProc( windowHandle, messageCode, wParam, lParam );

}

// Called when the window needs to be painted
void MyWindow::onWindowPaint( HWND windowHandle ) {

	// Create the Direct2D resources if they have not been created yet
	this->createGraphicsResources();

	// Get the current size of the render target, which is changed whenever the window is resized
	D2D1_SIZE_F renderTargetSize = this->renderTarget->GetSize();

	// The positions to use for draw the rectangle
	D2D1_RECT_F rectangleArea = D2D1::RectF(
		50.0f, // Left
		50.0f, // Top
		renderTargetSize.width - 50.0f, // Right
		renderTargetSize.height - 50.0f // Bottom
	);

	// Fill structure with data about the paint request (what area needs painting), and start the painting code
	// https://docs.microsoft.com/en-us/windows/win32/learnwin32/painting-the-window
	PAINTSTRUCT paintData;
	if ( BeginPaint( windowHandle, &paintData ) == NULL ) {
		consoleError( "Failed to begin painting!" );
		DestroyWindow( windowHandle ); // Close the window
		return;
	}

	// Paint a simple rectangle, this fills the region needing an update with a single system-defined background color
	// This function is part of the Graphics Device Interface (GDI), which is what Windows used for ages, until Windows 7 which introduced Direct2D
	//FillRect( displayDeviceContextHandle, &paintData.rcPaint, ( HBRUSH ) ( COLOR_WINDOW + 1 ) );

	// Start the drawing code
	this->renderTarget->BeginDraw();

	// Clear everything (fill with a color)
	// https://docs.microsoft.com/en-us/windows/win32/direct2d/id2d1rendertarget-clear
	this->renderTarget->Clear( D2D1::ColorF( D2D1::ColorF::LightGray, 1.0f ) );

	// Fill a rectangle using the gradient brush
	// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nn-d2d1-id2d1solidcolorbrush#examples
	this->renderTarget->FillRectangle( rectangleArea, this->gradientBrushFill );

	// Draw a rectangle outline using the solid brush
	// https://docs.microsoft.com/en-us/windows/win32/direct2d/getting-started-with-direct2d#step-5-draw-the-rectangle
	this->renderTarget->DrawRectangle( rectangleArea, this->solidBrushOutline );

	// Draw a circle outline
	this->renderTarget->DrawEllipse(
		D2D1::Ellipse(
			D2D1::Point2F( renderTargetSize.width / 2.0f, renderTargetSize.height / 2.0f ), // Position in the middle
			75.0f, 75.0f // The circle radius (X, Y)
		),
		this->solidBrushOutline, // Use the outline brush
		3.0f // The width of the outline (stroke)
	);

	// Draw some text
	// https://docs.microsoft.com/en-us/windows/win32/Direct2D/how-to--draw-text
	this->renderTarget->DrawTextW(
		L"Hello World!", // The text to draw
		12, // The length of the text
		this->writeTextFormat, // The text formatter resource
		rectangleArea, // Position of the text
		this->solidBrushText // Use the text brush
	);

	// End the drawing code
	HRESULT drawResult = renderTarget->EndDraw();

	// Discard the graphics resources if we need to re-create them (display changed, resolution changed, graphics device disconnected, etc.)
	if ( drawResult == D2DERR_RECREATE_TARGET ) {
		this->releaseGraphicsResources();

	// Close the window if any other error occurred
	} else if ( FAILED( drawResult ) ) {
		consoleError( "Failed to end Direct2D drawing! (%l)", drawResult );
		DestroyWindow( windowHandle );
		// No return because we still want to end the painting code
	}

	// End the painting code, this clears the update region & signals to Windows that the painting is complete
	EndPaint( windowHandle, &paintData );

}

// Called when the window is resized
void MyWindow::onWindowResize( HWND windowHandle, UINT type, UINT width, UINT height ) {

	// Update the size of the render target
	if ( this->renderTarget != NULL ) {
		this->renderTarget->Resize( D2D1::SizeU( width, height ) );
	}

	// Display a message to the console
	consoleOutput( "Window resized to %i by %i.", width, height );

}

// Called when the window is destroyed
void MyWindow::onWindowDestroy( HWND windowHandle ) {

	// Discard Direct2D resources
	this->releaseGraphicsResources();

	// Exit the message loop by pushing a quit message onto the message queue, which causes GetMessage() to return 0 and thus the loop ends
	PostQuitMessage( 0 );

	// Display a message to the console
	consoleOutput( "Window destroyed." );

}
