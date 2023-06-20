/**
 * @brief header file for EditConfig, a subclass of Parser
 */
#ifndef E_D_I_T_C_O_N_F_I_G__H
#define E_D_I_T_C_O_N_F_I_G__H
#include "Parser.h"

enum Wanted { WANT_UART_ENABLED,  WANT_SPI_ENABLED, WANT_UNKNOWN };
typedef enum Wanted wanted_t ;

class EditConfig:public Parser {
 private:
  bool isAComment;         // Is this line marked as comment?

  char *posOfFirstWord;      // Where is hash if comment? NULL if none.
  char *posOfEquals;       // Where is the '=' sign?
  
  Wanted wantedConfig;     // What do we want to configure this as?
  bool haveSPI;
  bool haveQCA7000;
  bool haveQCA7000_UART;
  bool haveUART;
  
  enum LineType { LT_DTOVERLAY=1, LT_DTPARAM, LT_ASSIGNMENT} lineType;
  enum ArgType  { AT_SPI=1, AT_QCA7000, AT_QCA7000_UART0, AT_UART, AT_OTHER } argType;

  bool triage(); // Parse the line down to its parts, determine if its an interesting line
  void makeComment(bool funct); // make (or unmake) a comment
 public:
  EditConfig();
  bool Begin(Wanted configureFileFor, const char *inFileName, const char *outFileName);
  bool editALine() override;
  bool appendToFile() override;
};
#endif
