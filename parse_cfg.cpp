/**
 * This parses the config file, located in /etc/default, and sets the resulting configuration
 * into static, internal variables.
 *
 * The config file is formatted as follows:
 *
 *   A '#' or ';' designates a comment until end-of-line
 * Blank lines, or lines only containing comments are ignored.
 *
 * paramters are formated as <varName>=value.
 *      <varName> is case-sensitive, letters followed by letters and numbers
 *      <value>   any comgbination of printable characters. 
 *  Leading and trailing spaces are ignored
 */
#include <syslog.h>
#include <regexp.h>
#include <stdio.h>
#include <errno.h>

using namespace std;
#define DEFAULT_FILE_NAME       "/etc/default/shutdownwatcher"
#define DEFAULT_CONFIG_TXT      "/boot/config.txt"
#define DEFAULT_SHUTDOWN_PIN GPIO17
#define DEFAULT_CONFIG_UART_PIN GPIO11
#define DEFAULT_HEARTBEAT_PIN   GPIO12
#define DEFAULT_HEARTBEAT_RATE  100

const std::string parseConfig::WHITESPACE = " \n\r\t\f\v";

/**
 * @brief initialize defaults
 */
parseConfig::parseConfig()
{
  shutdown_pin     = DEFAULT_SHUTDOWN_PIN;
  config_uart_pin  = DEFAULT_CONFIG_UART_PIN;
  config_led_pin   = DEFAULT_HEARTBEAT_PIN;
  config_heartbeat = DEFAULT_HEARTBEATE_RATE;
  config_file_name = DEFAULT_CONFIG_FILE;

/**
 * @brief Standard destructor
 */
parseConfig::~parseConfg()
{
}



std::string parseConfig::ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string parseConfig::rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string parseConfig::trim(const std::string &s) {
    return rtrim(ltrim(s));
}


// Convert a string to an int, do a return if not a vald conversion
#define CONVNUM								\
  try {res=strtol(value);}						\
  catch(...) {								\
    syslog("Bad value for paramter %s at line %d", _pname, _lineno);	\
    return; }

/**
 * For the given pname, set the indicated value.
 * errors are logged: cant find paramter, invalid value.

 * @param pname - name of the paramter
 * @param value - value for that paramter
 * @param lineno - the line number in the config file. Used for error msgs.
 */
void setValues(const string pname, const string value, int lineno)
{
 
  int res;
  if (0 == pname.compare("HEARTBEAT")
    {
      config_file_name = value;

    } else

      if ( 0 == pname.compare("SHUTDOWN"))
    {
      res=CONVNUM;
      config_shutdownPin=res;
      
    } else if (0 == pname.compare("UARTPIN"))
    {
      res=CONVNUM;
      config_uartPin = res;
      
    } else  if (0 == pname.compare("LEDPIN"))
    {
      res=CONVNUM;
      config_ledPin = res;
      
    } else if (0 == pname.compare("HEARTBEAT"))
    {
      res=COVNUM;
      config_heartbeat_rate=res;
    }

  return;      
}



  
/**
 * @brief PARSE a string for <name>=<value>
 
 * @param line[in] - the line to parse. It is not altered.
 * @oaram lineno   - the line number (used for error messages)
 * @Return none
 *
 * NOTE: Not thread safe!
 */
  void parseConfig::parseALine(const string  &line, int lineno)
{
  string wrkstr(line);
  size_t pos;
  
  string parseConfig::string pname;
  string parseConfig::string arg;

  
  // elimnate comments
  pos=wrkstr.find_first_of("#;");
  if (pos!= string::npos)   wrkstr.erase(pos, string::npos);
  
  // find '='
  pos = wrkstr.find("=");
  if (pos==string::npos)
    {
      printf("Did not find '='\n");
      return;
    }
  pname=trim(wrkstr.substr(0, pos));
  arg=trim(wrkstr.substr(pos+1, string::npos));
  printf("Have name = :%s:   arg = :%s:\n", pname.c_str(), arg.c_str());
  setValues(pname, arg, lineno)
}



  /**
   * @brief Parse the config file.
   * If fname is empty, use default config file.
   *  Default values are used if parsing fails.
   *
   * @fname: the name of the config file (/etc/default/shutdownwatcher)
   * @return: true normally, false if error
   */
  bool parseConfig::begin(int argc, const char *arg)
  {
    char name[256];
    FILE *infile;
  
    if (argcnt == 2)
      {
	if (strlen(arg[1])>=sizeof(name))
	  {
	    syslog("file name argument is longer than 255 characters");
	    return;
	  }
	strcpy(name, arg[1]);
      } else {
      strcpy(name, DEFAULT_FILE_NAME);
    }


    infile=fopen(fname, "r");
    if (infile==NULL)
      {
	syslog("shutdownwatcher:Open on file %s failed: %s", name, strerror(errno));
	return(false);
      }

    char *line=NULL;
    size_t len = 0;
    ssize_t nread;
    while(nread = getline(&line, &len, infile)) != -1)
    {
      parseALine( (string)line);
    }
    return;
}
