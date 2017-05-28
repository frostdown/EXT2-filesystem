
#include "../header/parser.h"



// goal turn raw input into processed commands

// ex format cmd1 (< infile) | cmd2 | cmd3 | cmd 4 (> outfile)

// becomes:
// processedCommands
//      firstPart
//          pipe
//              cmd1, cmd2, cmd3, cmd4


// remember commands can also be trivial commands.
void parseCommands(char* input, Pipe** result)
{
    *result = allocPipe();

    // find pipes
    if (strchr(input, '|') == NULL)
    {
        // no pipes, set up for single command
        processSingleCommand(input, &(*result)->command);
    }else
    {
        // tokenize all commands
        char commands[10][256];
        int i = 0;
        memset(commands, 0, 256*10);

        char* currentCommand = strtok(input, "|");
        strcpy(commands[i], currentCommand);

        while (currentCommand = strtok(0, "|"))
        {
            i++;
            strcpy(commands[i], currentCommand);
        }

        
        // process first command
        Pipe* nextPipe;
        *result = allocPipe();

        i = 0;
        currentCommand = commands[i];
        processSingleCommand(currentCommand, &((*result)->command));

        i++;
        currentCommand = commands[i];
        
        while(strcmp(currentCommand, "") != 0)
        {
            // insert new command at front of pipe
            nextPipe = allocPipe();
            nextPipe->leftPipe = *result;
            *result = nextPipe;

            // process command
            processSingleCommand(currentCommand, &((*result)->command));

            // increment command
            i++;
            currentCommand = commands[i];
        }
    }
}

void setNullIfExists(char* c)
{
    if (c != NULL) 
    {
        while (*c != ' ' && *c != '\0')
        {
            *c = '\0';
            c++;
        }
    }
}

void processSingleCommand(char* command, Command* result)
{
    memset(result, 0, sizeof(Command));

    char* s;
    if (strlen(command) == 0) return;

    // find the output append redirect if it exists
    char* outAppendRedirect = strstr(command, ">>");
    char* outRedirect = NULL;
    if (outAppendRedirect == NULL)
    {
        // find the output redirect if it exists
        outRedirect = strchr(command, '>');
    }

    // find the input redirect if it exists.
    char* inRedirect = strchr(command, '<');

    // set redirect characters to null
    setNullIfExists(inRedirect);
    setNullIfExists(outRedirect);
    setNullIfExists(outAppendRedirect);

    // process and save redirect strings
    processRedirectString(inRedirect, &result->inRedirect);
    processRedirectString(outRedirect, &result->outRedirect);
    if (outAppendRedirect != NULL)
    {
        result->outRedirect.isAppend = 1;
        outAppendRedirect++; // increment out append pointer to avoid the second >
    }
    processRedirectString(outAppendRedirect, &result->outRedirect);

    // handle command string
    // break up filepath
    int i = 0;
    s = strtok(command, " ");
    strcpy(result->tokenizedCommand[i], s);
    i++;

    while ((s = strtok(0, " ")) != NULL)
    {
        strcpy(result->tokenizedCommand[i], s);
        i++;
    }
}

void processRedirectString(char* string, IORedirect* redirect)
{
    if (string != NULL)
    {
        // remove forward white spaces
        string++;
        while (*string == ' ') string++;

        // save to redirect
        redirect->haveRedirect = 1;
        strcpy(redirect->filename, string);
    }
}

