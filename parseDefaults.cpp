/**
 * This parses the defaults file, (normally located in /etc/default/shutdownwatcher), and sets the 
 * resulting configuration into static, internal variables.
 *
 * The defaults file is formatted as follows:
 *
 *   A '#' or ';' designates a comment until end-of-line
 * Blank lines, or lines onlY containing comments are ignored.
 *
 * paramters are formated as <varName>=<value>
 *      <varName> is case-sensitive, letters followed by letters and numbers
 *      <value>   any comgbination of printable characters. 
 *  Leading and trailing spaces are ignored
 */
#include <string>
#include <string.h>
#include <syslog.h>
#include <regex.h>
#include <stdio.h>
#include <errno.h>
#include "parseDefaults.h"

#define NOT_A_PI

using namespace std;

#ifdef NOT_A_PI
#define DEFAULT_MODE_FILE       "/tmp/shutdownLastMode.txt"
#define DEFAULT_UART_FILE       "/boot/config.txt_uart"
#define DEFAULT_SPI_FILE        "/boot/config.txt_spi"
#define DEFAULT_CONFIG_TXT      "/boot/config.txt"
#define DEFAULT_SHUTDOWN_PIN    17
#define DEFAULT_CONFIG_UART_PIN 11
#define DEFAULT_HEARTBEAT_PIN   12
#define DEFAULT_HEARTBEAT_RATE  100

#else
#define DEFAULT_MODE_FILE       "/tmp/shutdownLastMode.txt"
#define DEFAULT_UART_FILE       "/boot/config.txt_uart"
#define DEFAULT_SPI_FILE        "/boot/config.txt_spi"
#define DEFAULT_CONFIG_TXT      "/boot/config.txt"
#define DEFAULT_SHUTDOWN_PIN    GPIO17
#define DEFAULT_CONFIG_UART_PIN GPIO11
#define DEFAULT_HEARTBEAT_PIN   GPIO12
#define DEFAULT_HEARTBEAT_RATE  100
#endif

static const char *WHITESPACE = " \n\r\t\f\v";

/**
 * @brief initialize defaults
 */
parseDefaults::parseDefaults()
{
  config_txt_modeFile    = DEFAULT_MODE_FILE;
  config_txt_uart         = DEFAULT_UART_FILE;
  config_txt_spi          = DEFAULT_SPI_FILE;
  config_txt_main         = DEFAULT_CONFIG_TXT;
  config_shutdownPin      = DEFAULT_SHUTDOWN_PIN;
  config_uartPin          = DEFAULT_CONFIG_UART_PIN;
  config_ledPin           = DEFAULT_HEARTBEAT_PIN;
  config_heartbeat_rate   = DEFAULT_HEARTBEAT_RATE;
}


/**
 * @brief Standard destructor
 */
parseDefaults::~parseDefaults()
{
}


std::string parseDefaults::ltrim(const std::string &s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string parseDefaults::rtrim(const std::string &s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string parseDefaults::trim(const std::string &s) {
  return rtrim(ltrim(s));
}


// Convert a string to an int, do a return if not a vald conversion
static int CONVNUM(const string _val, const string &_pname, int &_lineno)
{
  int res=0;
  try {
    res = strtol(_val.c_str(), nullptr, 10);
  }
  catch(...) {
    syslog(LOG_CONS|LOG_DAEMON, "Bad value for paramter %s at line %d", _pname.c_str(), _lineno);
    res=0;
  }
  return(res);
}

/**
 * For the given pname, set the indicated value.
 * errors are logged: cant find paramter, invalid value.

 * @param pname - name of the paramter
 * @param value - value for that paramter
 * @param lineno - the line number in the config file. Used for error msgs.
 */
void parseDefaults::setValues(const string pname, const string value, int lineno)
{
 
  int res;
  if (0 == pname.compare("CONFIGSPI"))
    {
      config_txt_spi.assign(value);

    } else if (0== pname.compare("CONFIGUART"))
    {
      config_txt_uart.assign(value);
      
    } else if (0 == pname.compare("CONFIGMAIN"))
    {
      config_txt_main.assign(value);
      
    } else if ( 0 == pname.compare("SHUTDOWN"))
    {
      res=CONVNUM(value, pname, lineno);
      config_shutdownPin=res;
      
    } else if (0 == pname.compare("UARTPIN"))
    {
      res=CONVNUM(value, pname, lineno);
      config_uartPin = res;
      
    } else  if (0 == pname.compare("LEDPIN"))
    {
      res=CONVNUM(value, pname, lineno);
      config_ledPin = res;
      
    } else if (0 == pname.compare("HEARTBEAT"))
    {
      res=CONVNUM(value, pname, lineno);
      config_heartbeat_rate=res;
      
    } else if (0 == pname.compare("CONFIGMODE"))
    {
      config_txt_modeFile.assign(value);
    }

  return;      
}


int parseDefaults::shutdownPin()
{
  return(config_shutdownPin);
}


int parseDefaults::uartPin()
{
  return( config_uartPin);
}


int parseDefaults::heartbeatPin()
{
  return( config_ledPin);
}


int parseDefaults::heartbeatRate()
{
  return(config_heartbeat_rate);
}


string parseDefaults::configtxt_main()
{
  return(config_txt_main);
}


string parseDefaults::configtxt_uart()
{
  return(config_txt_uart);
}


string parseDefaults::configtxt_spi()
{
  return(config_txt_spi);
}

string parseDefaults::configtxt_modeFile()
{
  return(config_txt_modeFile);
}
  
/**
 * @brief PARSE a string for <name>=<value>
 
 * @param line[in] - the line to parse. It is not altered.
 * @oaram lineno   - the line number (used for error messages)
 * @Return none
 *
 * NOTE: Not thread safe!
 */
void parseDefaults::parseALine(const string  &line, int lineno)
{
  string wrkstr(line);
  size_t pos;
  
  string pname;
  string arg;

  
  // elimnate comments
  pos=wrkstr.find_first_of("#;");
  if (pos!= string::npos)   wrkstr.erase(pos, string::npos);
  
  // find '='
  pos = wrkstr.find("=");
  if (pos==string::npos)
    { // no '='
      return;
    }
  pname=trim(wrkstr.substr(0, pos));
  arg=trim(wrkstr.substr(pos+1, string::npos));
  // printf("Have name = :%s:   arg = :%s:\n", pname.c_str(), arg.c_str());
  setValues(pname, arg, lineno);
}



/**
 * @brief Parse the 'defaults' file.
 * If fname is empty, use default config file.
 *  Default values are used if parsing fails.
 *
 * @fname: the name of the config file (/etc/default/shutdownwatcher)
 * @return: true normally, false if error
 */
void parseDefaults::begin(const string &defaultFileName)
{
  FILE *defaufFile;
    
  defaufFile=fopen(defaultFileName.c_str(), "r");
  if (defaufFile==NULL)
    {
      syslog(LOG_CONS|LOG_DAEMON,
	     "shutdownwatcher:Open on file %s failed: %s",
	     defaultFileName.c_str(), strerror(errno));
      return;
    }

  char *line=NULL;
  size_t len = 0;
  ssize_t nread;
  int lineno=0;
  
  while( (nread = getline(&line, &len, defaufFile)) != -1)
    {
      lineno++;
      parseALine( (string)line, lineno);
    }
  return;
}
