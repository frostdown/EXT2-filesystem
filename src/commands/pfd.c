
#include "../../header/commands/pfd.h"

int pfd(Command* command)
{
    printf("fd  mode    offset  INODE\n");
    printf("-------------------------\n");

    for (int i = 0; i < NFD; i++)
    {
        OFT* oft;
        oft = running->fd[i];

        // check FD exists
        if (oft == NULL)
        {
            break;
        }

        // print out the fd information
        printFD(oft, i);
    }
}

void printFD(OFT* oft, int fdNum)
{
    char mode[128] = {0};
    switch(oft->mode)
    {
        case 0: // read
            strcpy(mode, "READ");
            break;
        case 1: // write
            strcpy(mode, "WRITE");
            break;
        case 2: // read-write
            strcpy(mode, "RW");
            break;
        case 3: // append
            strcpy(mode, "APPEND");
            break;
    }

    int pdev = oft->mptr->dev;
    int pino = oft->mptr->ino;

    printf("%d   %s    %d      [%d, %d]\n", fdNum, mode, oft->offset, pdev, pino);

}
