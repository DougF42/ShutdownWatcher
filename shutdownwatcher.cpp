/**
 * We watch for a button press - if seen, then
 * we call the 'shutdown' program and quit.
 *
 * This uses wiring pi library - see 'http://wiringpi.com'
 *
 * Uses the 'interrupt' features!
 */

#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>


//IF defined, we wait for an interrupt.
#define USE_INTERRUPT
// #define TESTMODE

// default GPIO pin
//#define DEF_SWITCH 14
#define DEF_SWITCH 18

using namespace std;

/**
 * This calls shutdown with the '-h now; argument.
 */
void callShutdown(void)
{
  #ifdef TESTMODE
  syslog(LOG_DAEMON|LOG_EMERG, "TEST MODE: Shutdown triggered");
  #else
  syslog(LOG_DAEMON|LOG_EMERG, "Shutdown button pushed. shutting down",NULL);
  int stat=system("shutdown -h now");
  #endif
  exit(0);
}

int main(int argc, char **argv) {
  int switchno = DEF_SWITCH;
//  	printf( "Hello world");
  syslog(LOG_DAEMON|LOG_EMERG, "Power button watcher started");
	wiringPiSetupGpio();   // Use broadcom GPIO pin numbers
	pinMode(switchno, INPUT);  // set switch to input mode
	pullUpDnControl (switchno, PUD_UP) ; // set Pull-up 50k resistor
	syslog(LOG_DAEMON|LOG_EMERG, "GPIO configured");
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
