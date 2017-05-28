

#include "../../header/commands/link.h"


int link(Command* command)
{
    char* filePathString = command->tokenizedCommand[1];
    char* linkPathString = command->tokenizedCommand[2];

    // get inumber of file to link to
    Path filePath;
    parseFilepath(filePathString, &filePath);
    DIR fileDir;
    findDir(&filePath, type_File, &fileDir);
    int file_fileType = fileDir.file_type;
    int linkInode = fileDir.inode;

    // go to parent of link file
    Path linkPath;
    INODE dummy;
    parseFilepath(linkPathString, &linkPath);
    int parentIno = getParentInode(&linkPath, &dummy);
    MINODE* pminode = iget(dev, parentIno);

    addDIR(pminode, linkInode, linkPath.baseName, file_fileType);

    iput(pminode);
}



