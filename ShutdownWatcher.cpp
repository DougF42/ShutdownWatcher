/**
 * We watch for a button press - if seen, then
 * we call the 'shutdown' program and quit.
 *
 * This uses wiring pi library - see 'http://wiringpi.com'
 *
 * Uses the 'interrupt' features!
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

// #define TEST_NOW
#define USE_INTERRUPT

// Pick a switch (GPIO Number)
#define SWITCH 14

using namespace std;

static bool startShutdown=false;

/**
 * This calls shutdown with the '-h now; argument.
 */
void callShutdown(void)
{
  startShutdown=true;
  int stat=system("shutdown -h now");
}

int main(int argc, char **argv) {
	printf( "Hello world\n");

	#ifdef TEST_NOW
	callShutdown();
	exit(0);
	#else

	wiringPiSetupGpio();   // Use broadcom GPIO pin numbers
	pinMode(SWITCH, INPUT);  // set switch to input mode
	pullUpDnControl (SWITCH, PUD_UP) ; // set Pull-up 50k resistor
#ifdef USE_INTERRUPT
	wiringPiISR(SWITCH, INT_EDGE_FALLING, callShutdown);
	while(1)
	  {
	    sleep(5); // wait for interrupt
	  }
#else
	while(1)
	{
		if (0 == digitalRead(SWITCH))
		{
			callShutdown();
			exit(0);
		}
		sleep(1);

	}
#endif
	#endif
	return 0;
}
