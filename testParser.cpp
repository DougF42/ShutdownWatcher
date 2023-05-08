#include <stdio.h>
#include "parse_cfg.h"

int main(int argc, char **argv)
{
  printf("Define parser\n");
  parseConfig parse;
  printf("Begin\n");
  parse.begin("./testConfigFile.txt");
  printf("Parse complete. Values:");
  printf("Shutdown pin: %d\n",   parse.shutdownPin() );
  printf("Uart pin: %d\n",       parse.uartPin() );
  printf("hear Beat pin: %d\n",   parse.heartbeatPin() );
  printf("heart Rate: %d\n",      parse.heartbeatRate() );
  printf("CONFIG file name: %s\n",parse.configtxt_name() ); 
	 
  printf("FINISHED\n");
}
