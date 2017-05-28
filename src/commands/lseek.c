
#include "../../header/commands/lseek.h"

int myLseek(Command* command)
{
    // get inputs
    int fileDescriptor = atoi(command->tokenizedCommand[1]);
    int position = atoi(command->tokenizedCommand[2]);


    // ensure file descriptor is valid
    if (fileDescriptor > NFD || fileDescriptor < 0) return -1;
    if (running->fd[fileDescriptor] == NULL) return -1;

    // get original position
    int originalPosition = running->fd[fileDescriptor]->offset;

    // ensure position is within range of the file
    if (position < 0) position = 0;
    int fileSize = running->fd[fileDescriptor]->mptr->INODE.i_size;
    if (position > fileSize) position = fileSize;

    // set OFT position
    running->fd[fileDescriptor]->offset = position;

    // return original position
    return originalPosition;
}


