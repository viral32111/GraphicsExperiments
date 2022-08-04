// My custom window class
#include "MyWindow.h"

// Creates the factory for creating other resources
void MyWindow::setupDirect2D() {

	// Create a factory, which is used to create Direct2D resources, there should only be one for the lifetime of the application
	// https://docs.microsoft.com/en-us/windows/win32/direct2d/getting-started-with-direct2d#step-2-create-an-id2d1factory
	HRESULT factoryResult = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->d2dFactory );

	// Do not continue if there was an issue creating the factory
	if ( FAILED( factoryResult ) || this->d2dFactory == NULL ) {
		std::cerr << "Failed to create the Direct2D factory: " << factoryResult << std::endl;
		ExitProcess( 1 );
		return;
	}

	// Create the graphics resources so they are ready for the first paint call
	this->createGraphicsResources();

}

// Creates the graphics resources (render target, brushes, etc.)
void MyWindow::createGraphicsResources() {

	// Do not continue if these resources have already been created
	if ( this->renderTarget != NULL || this->solidBrushOutline != NULL || this->gradientBrushFill != NULL ) return;

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
		std::cerr << "Failed to create the Direct2D render target: " << renderTargetResult << std::endl;
		ExitProcess( 1 );
		return;
	}

	// Create a solid brush for painting (RGB values can also be given in hexadecimal notation)
	// https://docs.microsoft.com/en-us/windows/win32/direct2d/getting-started-with-direct2d#step-4-create-a-brush
	HRESULT solidBrushResult = this->renderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Black, 1.0f ), &this->solidBrushOutline );

	// Do not continue if there was an issue creating the solid brush
	if ( FAILED( solidBrushResult ) || this->solidBrushOutline == NULL ) {
		std::cerr << "Failed to create the Direct2D solid brush: " << solidBrushResult << std::endl;
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
		std::cerr << "Failed to create the Direct2D gradient stop collection: " << gradientCollectionResult << std::endl;
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
		std::cerr << "Failed to create the Direct2D linear gradient brush: " << gradientBrushResult << std::endl;
		ExitProcess( 1 );
		return;
	}

}

// Discards the graphics resources (render target, brushes, etc.)
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

}

// Discards the factory, and graphics resources
void MyWindow::releaseDirect2D() {

	// Discard all graphics resources first
	this->releaseGraphicsResources();

	// Discard the factory
	if ( this->d2dFactory != NULL ) {
		this->d2dFactory->Release();
		this->d2dFactory = NULL;
	}

}
