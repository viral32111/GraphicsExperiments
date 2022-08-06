#pragma once

// Standard string library
#include <string>

// Variadic arguments
#include <cstdarg>

// Windows API
#include <Windows.h>

// Our custom functions
void consoleCreate( std::string );
void consoleClose( std::string );
void consoleOutput( std::string, ... );
void consoleError( std::string, ... );
