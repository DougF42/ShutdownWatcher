/**
 * @file: parse the config file, provide configurable values:
 *
 * SHUTDOWN= <gpio_pin_no>              ; default nn
 * UARTPIN = <gpio_pin_no>              ; default nn
 * LEDPIN  = <gpio_pin_no>              ; default nn
 * CONFIGFILE = <name of config.txt>    ; /boot/config.txt
 * HEARTBEAT = <rate, in millisecs>     ; 50 milliseconds
 *
 */
#include <iostream>
#include <string>
#ifndef P_A_R_S_E__C_F_G
#define P_A_R_S_E__C_F_G
using namespace std;

class parseDefaults{
 private:
  //  static const std::string WHITESPACE;
  int config_shutdownPin;    // What pin to trigger shutdown?
  int config_uartPin;        // what pin to configure uart/spi mode for qca7000
  int config_ledPin;         // What pin for heartbeat?
  int config_heartbeat_rate;  // How long (msecs) per pulse? will use 50% duty cycle.
  string config_file_name;   // Name of the config.txt file.
  
  void setValues(const string pname, const string value, int lineno);
  
 public:
  parseDefaults();
  ~parseDefaults();
  void begin(const string &defaultFileName);
  std::string ltrim(const std::string &s);
  std::string rtrim(const std::string &s);
  std::string trim(const std::string &s);
  
  void parseALine(const string &line, int lineno);
  int shutdownPin();
  int uartPin();
  int heartbeatPin();
  int heartbeatRate();
  string configtxt_name();
};
#endif
