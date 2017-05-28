#include "../../header/commands/rmdir.h"

int rmdir(Command *command)
{
    if(command->tokenizedCommand[1][0] == 0)
    {
        printf("Error no Directory name specified!\n");
        return;
    }
    char *name = NULL;
    Path filePath;
    INODE *result;
    INODE dummy;
    MINODE *parent;
    MINODE *child;

    SearchValue sv;

    parseFilepath(command->tokenizedCommand[1], &filePath);

    int parentIno = getParentInode(&filePath, &dummy);
    int childIno = findInode(&filePath, type_Directory, &dummy);

    child = iget(dev, childIno);
    parent = iget(dev, parentIno);
    child->refCount = 1;

    if(child->INODE.i_links_count > 2)
    {
        printf("Cannot delete dir not empty!\n");
        return;
    }


    sv.name = filePath.baseName;
    removeDIR(parentIno, sv, Search_Name);

    // get INODE of ino into dbuf[ ]
    int blk = (parent->ino - 1) / 8 + start_iblock; // iblock = Inodes start block #
    int disp = (parent->ino - 1) % 8;
    //printf("iget: ino=%d blk=%d disp=%d\n", ino, blk, disp);
    char parent_buff[BLKSIZE] = {0};
    get_block(dev, blk, parent_buff);
    ip = (INODE *)parent_buff + disp;
    // copy INODE to mp->INODE
    parent->INODE = *ip;

    int x = 0;
    while (child->INODE.i_block[x] != 0)
    {
        deallocateblock(child->INODE.i_block[x]);
        child->INODE.i_block[x] = 0;
        x++;
    }
    
    iput(child);
    int child_block_number = (((childIno)-1) / 8) + start_iblock;

    deialloc(child_block_number);
}