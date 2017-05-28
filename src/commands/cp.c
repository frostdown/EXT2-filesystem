#include "../../header/commands/cp.h"

int mycp(Command *command)
{

    Path src;
    char *srcFilepath = command->tokenizedCommand[1];
    parseFilepath(srcFilepath, &src);
    int inumsrc = 0;
    inumsrc = findInodeIndex(&src, type_File);
    if (inumsrc == 0)
    {
        printf("cant find source file!\n");
    }

    Path dst;
    char *dstFilepath = command->tokenizedCommand[2];
    parseFilepath(dstFilepath, &dst);
    int inumdst = 0;
    inumdst = findInodeIndex(&dst, type_File);
    if (inumdst == 0)
    {
        printf("cannot find destination, creating file!\n");
        strcpy(command->tokenizedCommand[1],command->tokenizedCommand[2]);
        creat_file(command);
        inumdst = findInodeIndex(&dst, type_File);
    }

    int fd = openhelper(inumsrc, 0);

    int gd = openhelper(inumdst, 2);

    int n = 0;
    char readbuf[BLKSIZE] = {0};

    while (n = readFile(fd, &readbuf, BLKSIZE))
    {
        mywrite(gd, readbuf, n); // notice the n in write()
    }
}