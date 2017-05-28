
#include "../header/dataStructures.h"



Pipe* allocPipe()
{
    Pipe* result = malloc(sizeof(Pipe));
    memset(&(result->command), 0, sizeof(Command));
    result->leftPipe = NULL;
}


void freePipe(Pipe* pipe)
{
    if (pipe->leftPipe != NULL)
    {
        freePipe(pipe->leftPipe);
        pipe->leftPipe = NULL;
    }

    free(pipe);
}


