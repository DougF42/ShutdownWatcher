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


//IF defined, we wait for an interrupt.
#define USE_INTERRUPT
#define TESTMODE
// default GPIO pin
#define DEF_SWITCH 14
#define DOSHUTDOWN 0

using namespace std;

/**
 * This calls shutdown with the '-h now; argument.
 */
void callShutdown(void)
{
  #ifdef TESTMODE
  printf("TEST MODE: Shutdown triggered\n");
  #else
  int stat=system("shutdown -h now");
  #endif
  exit(0);
}

int main(int argc, char **argv) {
  int switchno = DEF_SWITCH;
  	printf( "Hello world\n");

	wiringPiSetupGpio();   // Use broadcom GPIO pin numbers
	pinMode(switchno, INPUT);  // set switch to input mode
	pullUpDnControl (switchno, PUD_UP) ; // set Pull-up 50k resistor
#ifdef USE_INTERRUPT
	wiringPiISR(switchno, INT_EDGE_FALLING, callShutdown);
	while(1)
	  {
	    sleep(100); // wait for interrupt
	  }
#else
	while(1)
	{
		if (0 == digitalRead(switchno))
		{
			callShutdown();
			exit(0);
		}
		sleep(1);

	}
#endif
	return 0;
}
