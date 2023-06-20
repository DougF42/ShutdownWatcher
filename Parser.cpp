/**
 *
 * @AUTHOR: 6/18/2023 by Doug Fajardo. 
 */
#include "Parser.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <errno.h>

using namespace std;

Parser::Parser()
{
  len =0;
  nread =0;
  lineno=0;
  line = nullptr;
}

Parser::~Parser()
{
  if (line != nullptr)   free(line);
  len=0;
  nread=0;
  lineno=0;
}

/**
 * Begin parsing a file
 * @param inFileName - name of input file to read
 * @param outFileName - name of output file to write results into
 * @return [boolean] true if successfull, false if any error.
 */ 
bool  Parser::Begin(const char *inFileName, const char *outFileName)
{
  FILE *inFileFd;
  FILE *outFileFd;
  
  inFileFd=fopen(inFileName, "r");
  if (inFileFd==NULL)
    {
      printf("** ERROR: Open for READ on file %s failed: %s",
             inFileName, strerror(errno));
      return(false);
    }

  outFileFd=fopen(outFileName, "w");
  if (outFileFd==NULL)
    {
      printf("** ERROR: Open for WRITE on  file %s failed: %s",
             outFileName, strerror(errno));
      fclose(inFileFd);
      return(false);
    }

  // Copy and edit in place
  while (readALine(inFileFd))
    {
      lineno++;
      if (editALine()) writeToFile(outFileFd);
    }
  
  // Any final words???
  while (appendToFile())
    {
      writeToFile(outFileFd);
    }
  
  fclose(inFileFd);
  fclose(outFileFd);
  return(true);
}

/**
 * @brief get the next line. 
 * The line is stored in the protected variables:  'line' is the actual line.
 * @returns true normally (should get another line), false if end of file or any error.
 */
bool Parser::readALine(FILE *filefd)
{  
  nread = getline(&line, &len, filefd);
  if (nread == -1) return(false);
  lineno++;
  return(true);
}

/**
 * @brief edit the line as needed.
 * Uses protected variables line, len, nread and lineno 
 * If more space is needed, then the line should be reallocated (malloc et all) and len updated to match the buffer len.
 * nread must also be updated to reflect the actual 'real' number of characters.
 * return true normally, false if a fatal error is detected
 */
bool Parser::editALine()
{
  // Simply returning true will effectivly 'copy' the file.
  return(true);
}

/**
 * @FILE: This is called after the input file is complely processed
 * It is used where we need to append additional lines that were not
 * otherwise found in the file.
 * It is continually called while it returns true (with a line being written
 * after each call).
 * @return  true if a line needs to be written.
 */
bool Parser::appendToFile()
{
  return(false);
}
/**
 * @brief write the 'line' to the output file.
 * This writes the content of the 'line' variable, a total  'nread' characters.
 */
bool Parser::writeToFile(FILE *outFileFD)
{
  size_t res=fwrite(line, 1, nread, outFileFD);
  if (res != nread)
    {
      return(false);
    }
  return(true);
}

/**
 * @BRIEF put a new line into the line buffer
 * This re-allocates memory if needed, and 
 * sets the nread to match the new entry.
 * @param newText the text to be added. Must
 *       include a newline char if needed.
 */
void Parser::addALine(const char *newText)
{
  size_t l=strlen(newText);
  if (len < l)
    {
      line=(char *)realloc(line, l+1);
      len=l+1;
    }
  strcpy(line, newText);
  nread=l;
  return;
}
