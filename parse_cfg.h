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

class parseConfig{
 private:
  int config_shutdownPin;    // What pin to trigger shutdown?
  int config_uartPin;        // what pin to configure uart/spi mode for qca7000
  int config_ledPin;         // What pin for heartbeat?
  int config_hearbeat_rate;  // How long (msecs) per pulse? will use 50% duty cycle.
  string config_file_name;   // Name of the config.txt file.
  
 public:
  parseLine();
  ~parseLine();
  void begin(string configFileName);
  std::string ltrim(const std::string &s);
  std::string rtrim(const std::string &s);
  std::string trim(const std::string &s);
  
  void parseALine(const string &line);
  int shutdownPin();
  int uartPin();
  int heartbeatPin();
  int hearbeatRate();
};
