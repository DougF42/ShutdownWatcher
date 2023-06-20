#include "EditConfig.h"
#include <stdlib.h>
#include <strings.h>
#include <string.h>

EditConfig::EditConfig()
{
  posOfFirstWord=NULL;
  posOfEquals   =NULL;
  wantedConfig    = WANT_UNKNOWN;
  isAComment      = false;
  lineType        = LT_ASSIGNMENT;
  argType         = AT_OTHER;
  haveSPI=false;
  haveQCA7000=false;
  haveQCA7000_UART=false;
  haveUART=false;
}



bool EditConfig::Begin(wanted_t configureFileFor, const char *inFileName, const char *outFileName)
{
  wantedConfig=configureFileFor;
  return( Parser::Begin(inFileName, outFileName) );
}

/**
 * @brief: make the current line into a comment, or into a 'real' line
 * This will work correctly reardless of the mode of the line upon entry
 * (e.g.: if funct==true and line is already a comment, then no action is taken).
 *
 * @param funct - true to make a comment, false to make into a 'real' line
 */
void EditConfig::makeComment(bool funct)
{
  // Sanity check - is the line already the way we want it?
  if ( (isAComment ==  funct) )  return;
  
  if (funct)
    { // Make a comment.
      if ( len < (nread+1) )
	{ // need more space
	  line = (char *)realloc(line, nread+10);	  
	}
	memmove(line+1, line, nread+1);
	*line='#';
	nread++;
	  
    } else
    { // remove comment - make into a 'real' line
        memmove(line, posOfFirstWord, strlen(line) );
    }
}


/**
 * This is called once per line.
 */
bool EditConfig::editALine()
{
  if (! triage())   return(true); ; // Ignore if not interesting
  if ( (lineType==LT_ASSIGNMENT) && (argType != AT_SPI) ) return(true);
      
  // WANT TO ENABLE SPI MODE
  switch(wantedConfig)
    {
    case(WANT_SPI_ENABLED):
      if ( (lineType==LT_DTPARAM)      && (argType==AT_SPI) )          makeComment(false);
      if ( (lineType==LT_DTOVERLAY)  && (argType==AT_QCA7000) )      makeComment(false);
      if ( (lineType==LT_DTOVERLAY)  && (argType==AT_QCA7000_UART0) ) makeComment(true);
      if ( (lineType==LT_ASSIGNMENT) && (argType=AT_UART) )
	{
	  *(posOfEquals+1) = '0';
	  makeComment(false);
	}
      break;
	
    case(WANT_UART_ENABLED):
      if ( (lineType==LT_DTPARAM)    && (argType==AT_SPI) )          makeComment(true);
      if ( (lineType==LT_DTOVERLAY) && (argType==AT_QCA7000) )      makeComment(true);
      if ( (lineType==LT_DTOVERLAY) && (argType==AT_QCA7000_UART0) ) makeComment(false);
      
      if ( (lineType==LT_ASSIGNMENT) && (argType=AT_UART) ) 
	{
	  *(posOfEquals+1)='1';
	  makeComment(false);
	}
      break;
      
    case(WANT_UNKNOWN):
      return(false);
      break;
    }

  return(true);
}

/**
 * @FILE: Put a new line into 'line'. Adjust 
/**
 * @FILE If we dont have a setting that is needed, append it    
 *
 * @Return true if we added a setting, false if no settings added
 */
#define DTOVERLAY_QCA7000 "dtoverlay=qca7000\n"
#define DTOVERLAY_QCA7000_UART "dtoverlay=qca7000-uart0\n"
#define DTPARAM_SPI     "dtparam=spi=on\n"
#define ENABLE_UART     "enable_uart=1\n"

bool EditConfig::appendToFile()
{
  bool res=false;
	 
  switch(wantedConfig)
    {
    case(WANT_SPI_ENABLED): // need SPI on, QCA7000
      if (!haveSPI)
	{
	  addALine(DTPARAM_SPI);
	  haveSPI=true;
	  res=true;
	} else  if (!haveQCA7000)
	{
	  addALine( DTOVERLAY_QCA7000);
	  haveQCA7000=true;
	  res=true;
	}
      break;
      
    case(WANT_UART_ENABLED): // need AT_QCA7000_UART0 and AT_UART
      if (!haveQCA7000_UART)
	{
	  addALine(DTOVERLAY_QCA7000_UART);
	  haveQCA7000_UART=true;
	  res=true;
	} else if (!haveUART)
	{
	  addALine(ENABLE_UART);
	  haveUART=true;
	  res=true;
	}
      break;
    }

  return(res);
}

/**
 * Parse a line, (vareiable line in base class) determine its basic components.
 * We are interrested in:
 * (1) It might be a comment - don't care
 * (2) Does it have 'dtoverlay=' or 'dtparam=' or 'enable_uart'?
 * (3) What is the word after the '=' (SPI, qca7000, qca7000-uart0 ?
 */
#define LT_OVERLAY_S "dtoverlay"
#define LT_PARAM_S   "dtparam"
#define AT_SPI_S     "spi"
#define AT_ENABLE_S  "enable_uart"
#define AT_QCA7000_S "qca7000"
#define AT_QCA7000_UART_S "qca7000-uart0"

bool EditConfig::triage()
{
  int lenOfWord=0;
  char word[30]; // plenty of space for 1 word -
  memset(word,0, sizeof(word));
  lineType = LT_ASSIGNMENT; // default
  argType = AT_OTHER;       // default
  // Is this a comment? skip the leading '#'
  char *posOfComment = index(line, '#');
  isAComment = posOfComment != NULL;
  posOfFirstWord = (posOfComment==NULL)?line : posOfComment+1;

  // Where is '=' separator?
  posOfEquals = index(posOfFirstWord, '=');
  if (posOfEquals == NULL) return(false); // not one of our lines!
  lenOfWord= posOfEquals - posOfFirstWord;
  
  // What type of Line?
  if ( 0==strncasecmp( posOfFirstWord, LT_OVERLAY_S, lenOfWord))
    {
      lineType = LT_DTOVERLAY;
      
    } else if (0==strncasecmp(posOfFirstWord, LT_PARAM_S, lenOfWord))
    {
      lineType = LT_DTPARAM;      
    } else 
    {
      lineType = LT_ASSIGNMENT;
    }

  // pointer to following word, if any
  strcpy( word, posOfEquals+1);   // a place where we can work
  char *stmp=strtok(word, "=,\n");
  
  // Given the type, check for an argument of interest..
  switch(lineType)
    {
    case(LT_DTPARAM):
      if (0 == strcasecmp(word, AT_SPI_S) )
	{
	  argType = AT_SPI;
	  haveSPI=true;
	}
      break;
      
    case(LT_DTOVERLAY):
      if (0==strcasecmp(word, AT_QCA7000_UART_S) )
	{
	  argType = AT_QCA7000_UART0;
	  haveQCA7000_UART=true;
	}
      
      else if (0==strcasecmp(word, AT_QCA7000_S) )
	{
	  argType = AT_QCA7000;
	  haveQCA7000=true;      
	}
    break;
    
    case(LT_ASSIGNMENT):
      argType=AT_OTHER;  // default
      memset(word,0,sizeof(word));
      strncpy(word, posOfFirstWord,posOfEquals-posOfFirstWord);
      if ( 0 == strcasecmp(word, AT_ENABLE_S) )
	{
	  argType=AT_SPI;
	  haveUART=true;
	}
      break;
    
    default:
      argType=AT_OTHER;
      return(false);
    }
  return(true);
 
}
