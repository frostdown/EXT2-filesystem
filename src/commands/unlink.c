
#include "../../header/commands/unlink.h"



int unlink(Command* command)
{
    // get path ready for processing
    Path path;
    parseFilepath(command->tokenizedCommand[1], &path);

    // get the inode of the parent
    INODE parentInode;
    int parentIno;
    parentIno = getParentInode(&path, &parentInode);

    // update the ref count of the child inode
    INODE childInode;
    int result = 0;
    result = findFileInode(&parentInode, path.baseName, type_File, &childInode);
    
    if (result == 0)
    {
        printf("unable to find file for unlinking!\n");
        return 1;
    }

    childInode.i_links_count--;

    if (childInode.i_links_count == 0)
    {
        // free up the inode and its blocks
        // TODO
    }

    // remove the DIR entry of the child inode
    SearchValue sv;
    sv.name = path.baseName;
    result = removeDIR(parentIno, sv, Search_Name);

    return result;
}

