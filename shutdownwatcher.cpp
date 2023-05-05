/**
 * Shutdown function:
 * We watch for a button press - if seen, then
 * we call the 'shutdown' program and quit.
 *
 * Change between SPI and UART mode
 *   (this goes out and enables/disables the dtparam=spi=on as needed,
 *    and also the enable_uart=1)
 * This uses wiring pi library - see 'http://wiringpi.com'
 *
 * Uses the 'interrupt' features!
 */

#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>


//IF defined, we wait for an interrupt.
#define USE_INTERRUPT

// #define TESTMODE


/**
 * This defines the switch we will monitor - we shut down when it goes low.
 */
#define SHUTDOWN_GPIO_PIN 18
#define PICK_UART_GPIO_PIN 25

/**
 * Identifies wether we want UART or SPI driver
 */
enum QCA_MODE {QCA_UART, QCA_SPI};
typedef enum QCA_MODE QCA_MODE_t;

/**
 * These define the config file to edit, and where to build the
 * working file.
 * We copy from CONFIG_TXT to CONFIG_TXT_WRK, makeing changes only
 * if they are necessary. When we are finished, IF there were changes
 * needed, we rename the CONFIG_TXT_WRK file to CONFIG_TXT.
 *
 * If no changes, then we simply delete CONFIG_TXT_WRK.
 *
 * The DELETE_WRK_FILE macro should be defined normally; undefine it
 *   for testing.
 */
#define CONFIG_TXT "/boot/config.txt"
#define CONFIG_TXT_WRK "/boot/config.txt.wrk"
#define DELETE_WRK_FILE
using namespace std;


/**
 * make sure the config lines are correct for the
 * chosen QCA7000 mode.
 * @param bfr point to null-terminated buffer
 * @param qca_mode do we want UART or SPI? 
 */
bool update_line(char *bfr, QCA_MODE_t qca_mode)
{
  // TODO
  return(false);
}


/**
 * This copies the config file, using 'update_line' to make
 * and needed modifications. 
 * @param qca_mode UART or SPI?
 * @return true if a change was made (will need to reboot)
 */
bool copyAndUpdateFile(QCA_MODE qca_mode)
{
  bool madeChanges=false ; // Did we make any changes?
  char buf[256];           // space for one line...
  int readlen;             // How many chars did we read?
  
  // OPEN CONFIG_TXT for read
  FILE *infile=fopen(CONFIG_TXT, "r");
  
  if (!infile)
    {     
      perror(CONFIG_TXT);
      exit(1);
    }

  // OPEN CONFIG_TXT_WRK for write
  FILE *wrkfile=fopen(CONFIG_TXT_WRK, "w");
  if (!wrkfile)
    {
      fclose(infile);
      perror(CONFIG_TXT_WRK);
      exit(2);
    }
  
  while(1)
    { // read, edit or copy  1 line.
      readlen = fread(buf, 1, sizeof(buf), infile);
      if (ferror(infile))
	{ // OOPS!
	  fclose(infile);
	  fclose(wrkfile);
#ifdef DELETE_WRK_FILE
	  remove(CONFIG_TXT_WRK);
#endif
	}
	  
      if (feof(infile))
	{ // all done
	  fclose(infile);
	  fclose(wrkfile);
	  break;
	}

      
      madeChanges |= update_line(buf, qca_mode);
      // write result to output
      fwrite(buf, 1, strlen(buf), wrkfile);
    }
  fclose(infile);
  fclose(wrkfile);
  if (madeChanges)
    {
      int res=rename(CONFIG_TXT_WRK, CONFIG_TXT);
      if (res!=0)
	{
	  perror("RENAME FAILED:");	  
	}
      
    }
  return(madeChanges);
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


int main(int argc, char **argv) {
  int switchno = SHUTDOWN_GPIO_PIN;
  //  	printf( "Hello world");
  QCA_MODE_t qcamode=QCA_SPI;  // an initial assumption
  
  syslog(LOG_DAEMON|LOG_EMERG, "Power button watcher started");
  wiringPiSetupGpio();   // Use broadcom GPIO pin numbers
  pinMode(switchno, INPUT);  // set switch to input mode
  pullUpDnControl (SHUTDOWN_GPIO_PIN, PUD_UP) ; // set Pull-up 50k resistor
  syslog(LOG_DAEMON|LOG_EMERG, "GPIO configured");

  pinMode(PICK_UART_GPIO_PIN, INPUT); // UART or SPI mode?
  pullUpDnControl(SHUTDOWN_GPIO_PIN, PUD_UP); 
  
  // TODO: Configure and check for SPI vrs UART mode
  if (digitalRead(PICK_UART_GPIO_PIN))
    {
      copyAndUpdateFile(QCA_UART);
    } else {
      copyAndUpdateFile(QCA_SPI);
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
