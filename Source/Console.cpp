#include "Console.h"

// The maximum size of the output buffer
const unsigned int OUTPUT_BUFFER_LENGTH = 1024;

// The standard input, output and error handles
HANDLE consoleStandardInput = NULL;
HANDLE consoleStandardOutput = NULL;
HANDLE consoleStandardError = NULL;

// Creates a new console window
void consoleCreate( std::string message ) {

	// Allocate the console window
	if ( AllocConsole() == FALSE ) {
		consoleError( "Failed to allocate console! (%lu)", GetLastError() );
		ExitProcess( 1 );
		return;
	}

	// Get the standard input, output and error handles
	consoleStandardInput = GetStdHandle( STD_INPUT_HANDLE );
	consoleStandardOutput = GetStdHandle( STD_OUTPUT_HANDLE );
	consoleStandardError = GetStdHandle( STD_ERROR_HANDLE );

	// Do not continue if any of the handles are invalid
	if ( consoleStandardInput == INVALID_HANDLE_VALUE || consoleStandardOutput == INVALID_HANDLE_VALUE || consoleStandardError == INVALID_HANDLE_VALUE ) {
		consoleError( "Failed to get standard input, output or error handle! (%lu)", GetLastError() );
		ExitProcess( 1 );
		return;
	}

	// Display the startup message
	consoleOutput( message );

}

// Release console resources
void consoleClose( std::string message ) {

	// Display the ending message
	consoleOutput( message );

	// Close the standard input, output and error handles
	if ( consoleStandardInput != NULL ) CloseHandle( consoleStandardInput );
	if ( consoleStandardOutput != NULL ) CloseHandle( consoleStandardOutput );
	if ( consoleStandardError != NULL ) CloseHandle( consoleStandardError );

	// Close the console window
	//FreeConsole();

}

// Write a message to a standard handle
void consoleWrite( HANDLE standardHandle, std::string messageFormat, va_list variadicValues ) {

	// Create buffer to hold the final message after formatting
	char messageFormatted[ OUTPUT_BUFFER_LENGTH ] = { 0 };

	// Add line ending to the end of the message
	messageFormat.append( "\n" );

	// Format the message using the variadic values
	int messageFormattedLength = vsprintf_s( messageFormatted, messageFormat.c_str(), variadicValues );

	// Write the message to the standard handle, or to the Visual Studio debug console if it is unavailable
	if ( standardHandle != NULL ) {
		WriteConsoleA( standardHandle, messageFormatted, ( DWORD ) messageFormattedLength, NULL, NULL );
	} else {
		OutputDebugStringA( messageFormatted );
	}

}

// Display a message to the standard output
void consoleOutput( std::string messageFormat, ... ) {

	// Start variadic arguments
	va_list variadicValues;
	va_start( variadicValues, messageFormat );

	// Write to standard output using variadic arguments
	consoleWrite( consoleStandardOutput, messageFormat, variadicValues );

	// End variadic arguments
	va_end( variadicValues );

}

// Display a message to the standard error
void consoleError(std::string messageFormat, ...) {

	// Start variadic arguments
	va_list variadicValues;
	va_start( variadicValues, messageFormat );

	// Write to standard error using variadic arguments
	consoleWrite( consoleStandardError, messageFormat, variadicValues );

	// End variadic arguments
	va_end( variadicValues );

}
