#include "Thread.h"

//bool stopThread = false;
//std::thread *theThread = NULL;

void threadCreate() {

	//int startingValue = 0;

	consoleOutput( "Creating thread..." );

	std::thread theThread;
	theThread = std::thread( threadTask ); // , 3
	theThread.join();

	/*std::thread myThread(threadTask);
	theThread = &myThread;*/

}

void threadStop() {

	/*consoleOutput("Stopping thread...");
	stopThread = true;

	consoleOutput( "Waiting for thread to finish..." );
	( *theThread ).join();*/

}

void threadTask() { //  int value 

	consoleOutput( "Hello from thread!" );

	/*consoleOutput("Thread started.");

	int counter = 0;

	while ( !stopThread ) {

		counter++;

		//consoleOutput( "Counter at: %d", counter );

	}

	consoleOutput( "Thread finished (%b).", stopThread );*/

}