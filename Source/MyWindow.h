#pragma once

// String manipulation
#include <string>

// Windows API
#include <Windows.h>

// Direct2D
#include <d2d1.h>
#include <d2d1helper.h>

class MyWindow {

	public:

		// Constructor & destructor
		MyWindow( LPCWSTR, LPCWSTR, UINT, UINT );
		~MyWindow();

		void setupWindowClass();
		void createMainWindow();
		void setupDirect2D();
		void pullWindowMessages();
		void releaseDirect2DResources();

	private:

		// Message receiver
		void windowProcedure();

		// Message handlers
		void onWindowResize( HWND windowHandle, UINT type, int width, int height );
		void onWindowDestroy( HWND windowHandle );
		void onWindowPaint( HWND windowHandle );

};
