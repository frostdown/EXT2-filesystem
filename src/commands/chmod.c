
#include "../../header/commands/chmod.h"


int chmod(Command* command)
{
    // get hex value command
    int16_t newMode = (int16_t)strtol(command->tokenizedCommand[1], NULL, 16);

    // get the inode of the file
    INODE inode;
    Path filepath;
    parseFilepath(command->tokenizedCommand[2], &filepath);

    int inum = findInode(&filepath, type_Any, &inode);
    if (inum == 0)
    {
        printf("file not found!\n");
        return 0;
    }

    MINODE* minode = iget(dev, inum);
    minode->INODE.i_mode = newMode;
    minode->dirty = 1;
    iwrite(minode);
    iput(minode);

    return 1;
}

