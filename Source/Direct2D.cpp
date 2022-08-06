// My custom window class
#include "MyWindow.h"

// Console functions
#include "Console.h"

// Creates the factories for creating other resources
void MyWindow::setupDirect2D() {

	// Create a Direct2D factory, which is used to create resources, there should only be one for the lifetime of the application
	// https://docs.microsoft.com/en-us/windows/win32/direct2d/getting-started-with-direct2d#step-2-create-an-id2d1factory
	HRESULT d2dFactoryResult = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->d2dFactory );

	// Do not continue if there was an issue creating the Direct2D factory
	if ( FAILED( d2dFactoryResult ) || this->d2dFactory == NULL ) {
		consoleError( "Failed to create the Direct2D factory! (%l)", d2dFactoryResult );
		ExitProcess( 1 );
		return;
	}

	// Create a DirectWrite factory, which is used for text
	HRESULT writeFactoryResult = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof( this->writeFactory ),
		( IUnknown ** ) &this->writeFactory
	);

	// Do not continue if there was an issue creating the DirectWrite factory
	if ( FAILED( writeFactoryResult ) || this->writeFactory == NULL ) {
		consoleError( "Failed to create the DirectWrite factory! (%l)", writeFactoryResult );
		ExitProcess( 1 );
		return;
	}

	// Display a message to the console
	consoleOutput( "Created Direct2D & DirectWrite factories." );

	// Create the graphics resources so they are ready for the first paint call
	this->createGraphicsResources();

}

// Creates the graphics (device-independent) resources (render target, brushes, etc.)
void MyWindow::createGraphicsResources() {

	// Do not continue if any of these resources have already been created
	if ( this->renderTarget != NULL ||
		this->solidBrushOutline != NULL ||
		this->gradientBrushFill != NULL ||
		this->writeTextFormat != NULL ) return;

	// Get the size of the window client area for drawing on
	RECT drawingArea;
	GetClientRect( this->windowHandle, &drawingArea );

	// Create a render target for the window, it can perform drawing operations & create drawing resources (brushes)
	// https://docs.microsoft.com/en-us/windows/win32/direct2d/getting-started-with-direct2d#step-3-create-an-id2d1hwndrendertarget
	HRESULT renderTargetResult = this->d2dFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(), // Remote display options, force hardware or software rendering, DPI (set to use default)
		D2D1::HwndRenderTargetProperties(
			this->windowHandle, // The handle to our top-level window
			D2D1::SizeU( drawingArea.right - drawingArea.left, drawingArea.bottom - drawingArea.top ) // Initial size of the drawing area
		),
		&this->renderTarget
	);

	// Do not continue if there was an issue creating the render target
	if ( FAILED( renderTargetResult ) || this->renderTarget == NULL ) {
		consoleError( "Failed to create the Direct2D render target! (%l)", renderTargetResult );
		ExitProcess( 1 );
		return;
	}

	// Create a solid brush for painting (RGB values can also be given in hexadecimal notation)
	// https://docs.microsoft.com/en-us/windows/win32/direct2d/getting-started-with-direct2d#step-4-create-a-brush
	HRESULT solidBrushResult = this->renderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Black, 1.0f ), &this->solidBrushOutline );

	// Do not continue if there was an issue creating the solid brush
	if ( FAILED( solidBrushResult ) || this->solidBrushOutline == NULL ) {
		consoleError( "Failed to create the Direct2D solid brush! (%l)", solidBrushResult );
		ExitProcess( 1 );
		return;
	}

	// Create a solid brush for text
	HRESULT solidBrushTextResult = this->renderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Blue, 1.0f ), &this->solidBrushText );
	if ( FAILED( solidBrushTextResult ) || this->solidBrushText == NULL ) {
		consoleError( "Failed to create the Direct2D solid brush for text! (%l)", solidBrushTextResult );
		ExitProcess( 1 );
		return;
	}

	// Create an array to hold two gradient stop structures
	const int GRADIENT_STOPS_COUNT = 2;
	D2D1_GRADIENT_STOP gradientStops[ GRADIENT_STOPS_COUNT ]{ 0 };

	// Define the starting & ending point colors of the gradient
	gradientStops[ 0 ].color = D2D1::ColorF( D2D1::ColorF::Yellow, 1.0f );
	gradientStops[ 0 ].position = 0.0f;
	gradientStops[ 1 ].color = D2D1::ColorF( D2D1::ColorF::Green, 1.0f );
	gradientStops[ 1 ].position = 1.0f;

	// Create a gradient stop collection using the above stops
	// https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1rendertarget-creategradientstopcollection(constd2d1_gradient_stop_uint32_d2d1_gamma_d2d1_extend_mode_id2d1gradientstopcollection)
	ID2D1GradientStopCollection *gradientStopCollection = NULL;
	HRESULT gradientCollectionResult = renderTarget->CreateGradientStopCollection(
		gradientStops, // The array of gradient stop structures
		GRADIENT_STOPS_COUNT, // The amount of stops in the array
		D2D1_GAMMA_2_2, // The color interpolation mode
		D2D1_EXTEND_MODE_CLAMP,
		&gradientStopCollection
	);

	// Do not continue if there was an issue creating the gradient stop collection
	if ( FAILED( gradientCollectionResult ) || gradientStopCollection == NULL ) {
		consoleError( "Failed to create the Direct2D gradient stop collection! (%l)", gradientCollectionResult );
		ExitProcess( 1 );
		return;
	}

	// Create a linear gradient brush for painting, using the gradient stops collection
	// https://docs.microsoft.com/en-us/windows/win32/Direct2D/how-to-create-a-linear-gradient-brush
	HRESULT gradientBrushResult = renderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties( // Determines direction of gradient
			D2D1::Point2F( 0.0f, 0.0f ), // Start at upper-left corner...
			D2D1::Point2F( drawingArea.bottom, drawingArea.right ) // ...end at lower-right corner.
		),
		gradientStopCollection, // The above gradient stop collection
		&this->gradientBrushFill // A reference to the brush
	);

	// Do not continue if there was an issue creating the linear gradient brush
	if ( FAILED( gradientBrushResult ) || gradientBrushFill == NULL ) {
		consoleError( "Failed to create the Direct2D linear gradient brush! (%l)", gradientBrushResult );
		ExitProcess( 1 );
		return;
	}

	// Create DirectWrite text format
	HRESULT textFormatResult = this->writeFactory->CreateTextFormat(
		L"Arial", // The name of the font to use
		NULL, // No collection of fonts
		DWRITE_FONT_WEIGHT_NORMAL, // Use standard font weight
		DWRITE_FONT_STYLE_NORMAL, // No additional font styling
		DWRITE_FONT_STRETCH_NORMAL, // Use standard stretching
		22.0f, // The font size
		L"", // The locale
		&this->writeTextFormat
	);

	// Do not continue if there was an issue creating the text format
	if ( FAILED( textFormatResult ) || writeTextFormat == NULL ) {
		consoleError( "Failed to create the DirectWrite text format! (%l)", textFormatResult );
		ExitProcess( 1 );
		return;
	}

	// Center the text horizontally & vertically
	this->writeTextFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_CENTER );
	this->writeTextFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );

	// Display a message to the console
	consoleOutput( "Created Direct2D & DirectWrite graphics resources." );

}

// Discards the graphics (device-independent) resources (render target, brushes, etc.)
// https://docs.microsoft.com/en-us/windows/win32/direct2d/getting-started-with-direct2d#step-6-release-resources
// https://docs.microsoft.com/en-us/windows/win32/medfound/saferelease
void MyWindow::releaseGraphicsResources() {

	// Discard the render target
	if ( this->renderTarget != NULL ) {
		this->renderTarget->Release();
		this->renderTarget = NULL;
	}

	// Discard the solid color brush
	if ( this->solidBrushOutline != NULL ) {
		this->solidBrushOutline->Release();
		this->solidBrushOutline = NULL;
	}

	// Discard the linear gradient brush
	if ( this->gradientBrushFill != NULL ) {
		this->gradientBrushFill->Release();
		this->gradientBrushFill = NULL;
	}

	// Discard the solid color brush for text
	if ( this->solidBrushText != NULL ) {
		this->solidBrushText->Release();
		this->solidBrushText = NULL;
	}

	// Discard the text formatter
	if ( this->writeTextFormat != NULL ) {
		this->writeTextFormat->Release();
		this->writeTextFormat = NULL;
	}

	// Display a message to the console
	consoleOutput( "Released Direct2D & DirectWrite graphics resources." );

}

// Discards the factories, and graphics resources
void MyWindow::releaseDirect2D() {

	// Discard all graphics resources first
	this->releaseGraphicsResources();

	// Discard the Direct2D factory
	if ( this->d2dFactory != NULL ) {
		this->d2dFactory->Release();
		this->d2dFactory = NULL;
	}

	// Discard the DirectWrite factory
	if ( this->writeFactory != NULL ) {
		this->writeFactory->Release();
		this->writeFactory = NULL;
	}

	// Display a message to the console
	consoleOutput( "Released Direct2D & DirectWrite factories." );

}
