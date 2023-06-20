#include <stdio.h>
#include "parseDefaults.h"

int main(int argc, char **argv)
{
  printf("Define parser\n");
  parseDefaults parse;
  printf("Begin\n\n");
  parse.begin("./testConfigFile.txt");
  printf("Parse complete. Values:");
  printf("MAIN CONFIG file: %s\n",parse.configtxt_main().c_str() );
  printf("UART CONFIG file: %s\n",parse.configtxt_uart().c_str() );
  printf("SPI  CONFIG file: %s\n",parse.configtxt_spi().c_str() );
  printf(" \n");
  printf("Shutdown pin: %d\n",   parse.shutdownPin() );
  printf("Uart pin: %d\n",       parse.uartPin() );
  printf("heart Beat pin: %d\n",  parse.heartbeatPin() );
  printf("heart Rate: %d\n",      parse.heartbeatRate() );	 
  printf("FINISHED\n\n");
  
}
