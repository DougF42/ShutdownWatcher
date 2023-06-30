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

// if true, then we log to syslog
#define USE_SYSLOG

// IF defined, we wait for an interrupt.
#define USE_INTERRUPT

#define DEFAULT_FILE_NAME "/etc/defaults/shutdownwatcher"
#define DEFAULT_TEST_FILE_NAME "/home/doug/ShutdownWatcher/testdir/testConfigFile.txt"
#define RUN_FILE_NAME "/var/run/shutdownwatcher/status"


#define UART_MODE 1
#define SPI_MODE  0
using namespace std;

#ifdef USE_SYSLOG
#define PRINT(_fmt) syslog(LOG_INFO| LOG_DAEMON, _fmt,
#define PRINTF(_fmt) syslog(LOG_INFO|LOG_DAEMON, _fmt)
#else
#define PRINT(_fmt) printf(_fmt,
#define PRINTF(_fmt) printf(_fmt)
#endif
string fname;
bool testModeFlag=false;
parseDefaults parser;

/** @brief help
 * Outputs command line syntax help
 */
void help()
{
  printf("THIS IS THE HELP INFO. RTFM, stupid!");
  return;
}
  

/** @brief
 * returns 0 for SPI mode, 1 for UART mode, -1 if any error (not present?)
 */
int checkRunState()
{
  char buf[5];
  FILE *file = fopen(parser.configtxt_modeFile().c_str(),"r");
  if (file == NULL)
    {
      return(-1); // no file
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
  FILE *file = fopen(parser.configtxt_modeFile().c_str(),"w");
  if (file == NULL) return; // do nothing on error
  
  if (state == UART_MODE)
    {
      fprintf(file,"UART\n");
    }  else
    {
      fprintf(file,"SPI\n");
    }
  fclose(file);
  return;
}


/** @breif call shutdown and exit
 * This calls shutdown with the '-h argument, UNLESS the
 * test mode flag is set. In that case it mearly announces
 * the call to shutdown and exits the program
 */		  
void callShutdown(void)
{
  if (testModeFlag)
    {
      PRINTF("TEST MODE: Shutdown indicated but NOT triggered.\n");
    }  else
    {
      syslog(LOG_DAEMON|LOG_EMERG, "Shutdown button pushed. shutting down",NULL);
      system("shutdown -h now");
    }
  
  exit(0);
}

/**
 * Decode command line arguments.
 * Only arguments are:
 *      '-t' (test mode) - output to sysout, announce but do not perform reboot.
 *      '?'  (get help)
 *      <filename> Configuration file name. Must not start with a '-' (dash). 
 *              Default is 'DEFAULT_FILE_NAME' flag.
 *
 * WE set the global variables:
 *    fname (the file name)
 *    testModeFlag (true if test mode requested)
 *
 * Params argc - number of arguments
 * Param  argv - list of arguments
 * @return 1 normally, 0 if we need 'help'
 */
bool parseCommandLine(int argc, char **argv)
{
  fname = DEFAULT_FILE_NAME;
  testModeFlag=false;
  if ((argc<=0) || (argc>3))
    {
      PRINTF("Wrong number of arguments\n");
      return (0); // wrong arg count
    }

  if ((argc>1) && (0==strcmp(argv[1],"?"))) return(0); // help
  for (int i=1;  i<argc; i++)
    {
      if (0 == strcmp(argv[i],"-t"))
	{
	  testModeFlag=true;
	  fname= DEFAULT_TEST_FILE_NAME ;
	} else
	{
	  fname=argv[i];
	}
    }
  return(1);
}


/**
 * MAIN entry point.
 * IF present, the first argument is the configuration 
 * file (default: /etc/default/shutdownwatcher)
 */ 
int main(int argc, char **argv) {
  #ifdef USE_SYSLOG
  openlog("SHUTDOWNWATCHER",LOG_NOWAIT, LOG_DAEMON);
  
  #endif
  PRINTF("Power button watcher started\n");
  if (0==parseCommandLine(argc, argv))
    {     
      help();
      return(1);
    }

  PRINT("Parse file %s\n") fname.c_str());
  parser.begin(fname);
  
  PRINTF("Parse complete. Values:\n");
  PRINT("MAIN CONFIG file: %s\n") parser.configtxt_main().c_str());
PRINT("UART CONFIG file: %s\n") parser.configtxt_uart().c_str() );
PRINT("SPI  CONFIG file: %s\n") parser.configtxt_spi().c_str() );
PRINT("RUN file is:      %s\n") parser.configtxt_modeFile().c_str());
PRINTF(" \n");
PRINT("Shutdown pin: %d\n")   parser.shutdownPin() );
PRINT("Uart pin: %d\n")       parser.uartPin() );
PRINT("heart Beat pin: %d\n")  parser.heartbeatPin() );
PRINT("heart Rate: %d\n")      parser.heartbeatRate() );	 
PRINTF("----\n\n");
  

  wiringPiSetupGpio();         // Use broadcom GPIO pin numbers
  pinMode(parser.shutdownPin(), INPUT);   // shut down pin - input
  pullUpDnControl (parser.shutdownPin(), PUD_UP) ; // set Pull-up 50k resistor
  
  pinMode(parser.heartbeatPin(), OUTPUT);   // LED PIN - output
  pullUpDnControl (parser.shutdownPin(), PUD_UP) ; // set Pull-up 50k resistor
  

  pinMode(parser.uartPin(), INPUT); // UART or SPI mode pin - INPUT
  pullUpDnControl(parser.uartPin(), PUD_UP); 
  
  // Check and configure UART or SPI mode if changed
  int curState = digitalRead(parser.uartPin()) == 0; // true if UART
  int oldState = checkRunState();
  string cmdStr;
  if ( oldState != curState)
    {  // change the state 
      if (curState)
	{ // UART mode
	  if (testModeFlag)
	    {
	      PRINTF("IN TEST MODE - would have set UART MODE\n");
	    } else
	    {
	      PRINTF("SETTING UART MODE");
	      cmdStr="cp ";
	      cmdStr.append(parser.configtxt_uart().c_str() );
	      cmdStr.append(" ");
	      cmdStr.append(parser.configtxt_main().c_str() );
	      system(cmdStr.c_str() );
	    }

	} else {
	// SPI mode
	if (testModeFlag)
	  {
	    PRINTF("IN TEST MODE - would have set SPI mode\n");
	  } else
	  {
	    PRINTF("SETTING SPI MODE");
	    cmdStr="cp ";
	    cmdStr.append(parser.configtxt_spi().c_str() );
	    cmdStr.append(" ");
	    cmdStr.append(parser.configtxt_main().c_str() );	  
	    system( cmdStr.c_str() );
	  }

      }
      writeRunState(curState);	  
      callShutdown(); // NOTE: This will exit.
    }
  
#ifdef USE_INTERRUPT
  wiringPiISR(parser.shutdownPin(),
	      INT_EDGE_FALLING, callShutdown);
#endif
  bool waveToggle=0;
  while(1)
    {
#ifndef USE_INTERRUPT
      if (0 == digitalRead(parser.shutdownPin()))
	{
	  callShutdown();
	  exit(0);
	}
#endif
      if (waveToggle == 1)
	{ // turn off the heartbeat
	  digitalWrite( parser.heartbeatPin(), 0);
	  waveToggle=0;
	} else
	{ // Turn on the heartbeat
	  digitalWrite( parser.heartbeatPin(), 1);
	  waveToggle=1;
	}
      delay(parser.heartbeatRate() );
    }
  return 0;
}
