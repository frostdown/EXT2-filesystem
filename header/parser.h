
#ifndef PARSER
#define PARSER

#include <stdbool.h>
#include <string.h>

#include "dataStructures.h"

// parse filepath
void parseFilepath(char* filepath, Path* result);

// parse commands and pipes
void parseCommands(char* input, Pipe** result);

void setNullIfExists(char* c);

void processSingleCommand(char* command, Command* result);

void processRedirectString(char* string, IORedirect* redirect);


#endif



