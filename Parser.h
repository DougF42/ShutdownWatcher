/**
 * @FILE: HEader for Parser library
 * @AUTHOR: 6/18/2023 by Doug Fajardo. 
 * 
 * This class is a 'framework' for editing a text file,
 * the design intent is to parse 'config.txt' files on the
 * raspberry pi.
 *
 * The functions 'readALine', editLine, and writeFile are all
 * virtual, and can be easily overridden in a subclass.
 *
 * The default read and writes are done using stream I/O,
 * one line at a time. The supplied editLine function does not
 * do any editing - if not overidden, we end up simply copying
 * the input file. 
 *
 */
#ifndef P_A_R_S_E_R__H
#define P_A_R_S_E_R__H
#include <string>
class Parser
{
 protected:
  char *line;    // The text that we just read
  size_t len;    // How much space is available in the 'line' buffer.
  ssize_t nread; // how many chars were read. 
  int lineno;    // The current line number.
  void addALine(const char *newText);
  
 public:
  Parser();
  ~Parser();
  bool Begin(const char *inFileName, const char *outFileName);
  
  virtual bool readALine(FILE *inFileFd);  // read the file line-by-line.
  virtual bool editALine();  // edit the content of the line. Writefile is invoked on result
  virtual bool appendToFile(); // called to append new lines to the file.

  virtual bool writeToFile(FILE *outFileFd);
};
#endif
