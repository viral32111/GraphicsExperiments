// Only include once when compiling
#pragma once

// Standard I/O
#include <iostream>

// Windows API
#include <Windows.h>

// Direct2D
#include <d2d1.h>

// Custom class to encapsulate everything
class MyWindow {

	// Only usable by this class
	private:

		// Initialized in the constructor
		const LPCWSTR WINDOW_CLASS_NAME;
		const LPCWSTR WINDOW_TITLE;
		const UINT WINDOW_WIDTH;
		const UINT WINDOW_HEIGHT;

		// Window
		HWND windowHandle = NULL;

		// Direct2D
		ID2D1Factory *d2dFactory = NULL;
		ID2D1HwndRenderTarget *renderTarget = NULL;
		ID2D1SolidColorBrush *solidBrushOutline = NULL;
		ID2D1LinearGradientBrush *gradientBrushFill = NULL;

		// Message receiver
		static LRESULT CALLBACK windowProcedure( HWND, UINT, WPARAM, LPARAM );

		// Message handlers
		void onWindowResize( HWND, UINT, UINT, UINT );
		void onWindowDestroy( HWND );
		void onWindowPaint( HWND );

	// Usable by anyone
	public:

		// Constructor & destructor
		MyWindow( LPCWSTR, LPCWSTR, UINT, UINT );
		~MyWindow();

		// Window
		void setupWindowClass( HINSTANCE );
		void createMainWindow( HINSTANCE, int );
		void pullWindowMessages();

		// Direct2D
		void setupDirect2D();
		void createGraphicsResources();
		void releaseGraphicsResources();
		void releaseDirect2D();

};
