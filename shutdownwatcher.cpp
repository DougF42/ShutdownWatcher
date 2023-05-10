/**
 * (1) Shutdown function:
 * We watch for a button press - if seen, then
 * we call the 'shutdown' program and quit.
 *
 * (2) Change between SPI and UART mode (On program startup):
 *   This goes out and enables/disables the dtparam=spi=on as needed,
 *    and also the enable_uart=1)
 *
 * (3) Heartbeat function. Once per second
 *
 * CONFIG:
 *   
 *
 * This uses wiring pi library - see 'http://wiringpi.com', including the 'interrupt' features!
 */

#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <time.h>
#include "parseDefaults.h"


// IF defined, we wait for an interrupt.
#define USE_INTERRUPT
#define DEFAULT_FILE_NAME "/etc/defaults/shutdownwatcher"
// IF defined, we dont actually reboot, but instead put out a stslog message.
#define TESTMOD

#define UART_MODE 1
#define SPI_MODE  0
using namespace std;

// Line 1 - UART or SPI only.
#define RUN_FILE_NAME "/var/run/shutdownwatcher/status"

/** @brief
 * returns 0 for SPI mode, 1 for UART mode, -1 if any error (not present?)
 */
int checkRunState()
{
  char buf[5];
  FILE *file = fopen(RUN_FILE_NAME,"w");
  if (file == NULL)
    {
      // TODO: error!;
    }
  size_t cnt=fread(buf, 1, 5, file);
  fclose(file);
  if (cnt !=5) return(-1);
  buf[4] = '\0';
  if (0 == strcmp(buf,"UART")) return(UART_MODE);
  if (0 == strcmp(buf,"SPI.")) return(SPI_MODE);
  return(-1);
}

/**
 * Writes the current state to the run file
 * @param state 0 for SPI mode, 1 for UART mode.
 */
void writeRunState(int  state)
{
  FILE *file = fopen(RUN_FILE_NAME,"w");
  if (file == NULL) return; // do nothing on error
  
  if (state == UART_MODE)
    {
      fprintf(file,"UART\n");
    }  else {
    fprintf(file,"SPI.\n");  
  }
  fclose(file);
  return;
}


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


/**
 * MAIN entry point.
 * IF present, the first argument is the configuration 
 * file (default: /etc/default/shutdownwatcher)
 */ 
int main(int argc, char **argv) {
  syslog(LOG_DAEMON|LOG_EMERG, "Power button watcher started");
  const char *fname=  DEFAULT_FILE_NAME ;
  parseDefaults parser;
  if (argc==2)
    {
      if ( 0 == strcmp(argv[2],"?"))
	{
	  help();
	  return(0);
	} else  {
	fname = argv[1]; 
      }
    }
  syslog(LOG_DAEMON|LOG_EMERG, "Parse file %s", fname);          
  parser.begin(fname);
  syslog(LOG_DAEMON|LOG_EMERG,"Parse complete. Values:");
  syslog(LOG_DAEMON|LOG_EMERG,"MAIN CONFIG file: %s\n",parse.configtxt_main().c_str() );
  syslog(LOG_DAEMON|LOG_EMERG,"UART CONFIG file: %s\n",parse.configtxt_uart().c_str() );
  syslog(LOG_DAEMON|LOG_EMERG,"SPI  CONFIG file: %s\n",parse.configtxt_spi().c_str() );
  syslog(LOG_DAEMON|LOG_EMERG," \n");
  syslog(LOG_DAEMON|LOG_EMERG,"Shutdown pin: %d\n",   parse.shutdownPin() );
  syslog(LOG_DAEMON|LOG_EMERG,"Uart pin: %d\n",       parse.uartPin() );
  syslog(LOG_DAEMON|LOG_EMERG,"heart Beat pin: %d\n",  parse.heartbeatPin() );
  syslog(LOG_DAEMON|LOG_EMERG,"heart Rate: %d\n",      parse.heartbeatRate() );	 
  syslog(LOG_DAEMON|LOG_EMERG,"FINISHED\n\n");
  

  wiringPiSetupGpio();         // Use broadcom GPIO pin numbers
  pinMode(parse.shutdownpin(), INPUT);   // shut down pin - input
  pullUpDnControl (parse.shutdownpin(), PUD_UP) ; // set Pull-up 50k resistor
  
  pinMode(parse.hearbeatPin(), OUTPUT);   // LED PIN - output
  pullUpDnControl (parse.shutdownpin(), PUD_UP) ; // set Pull-up 50k resistor
  

  pinMode(parse.uartPin(), INPUT); // UART or SPI mode pin - INPUT
  pullUpDnControl(parse.uartPin(), PUD_UP); 
  
  // Check and configure UART or SPI mode if changed
  int oldState = checkRunState();
  int curState = digitalRead(parse.uartPin()) == 0; // true if UART
  if ((oldState<0) || (oldState != curState)
    {  // change the state 
      if (curState)
	{ // UART mode
	  system("cp",parse.configtxt_uart().c_str(), parse.configtxt.main().c_str());
	  writeRunState(parse.uartPin());
	} else {
	// SPI mode
	system("cp", parse.configtxt_spi().c_str(),  parse.configtxt.main().c_str());
	writeRunState(parse.uartPin());
    }
    // TODO: REBOOT IF ANY CHANGE?

  
#ifdef USE_INTERRUPT
  wiringPiISR(SHUTDOWN_GPIO_PIN, INT_EDGE_FALLING, callShutdown);
  while(1)
    {
      sleep(100); // wait for interrupt
    }
#else
  while(1)
    {
      if (0 == digitalRead(SHUTDOWN_GPIO_PIN))
	{
	  callShutdown();
	  exit(0);
	}
      sleep(1);

    }
#endif
  return 0;
}
