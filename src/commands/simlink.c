#include "../../header/commands/simlink.h"

int simlink(Command *command)
{
    //source
    char *filePathString = command->tokenizedCommand[1];
    //destination -> includes the parent
    char *linkPathString = command->tokenizedCommand[2];

    // get inumber of file to link to
    Path filePath;
    parseFilepath(filePathString, &filePath);
    DIR fileDir;
    if (findDir(&filePath, type_File, &fileDir) == 0)
    {
        printf("Pathname not found, link aborted!\n");
    }

    // go to destination
    Path linkPath;
    INODE dummy;
    parseFilepath(linkPathString, &linkPath);
    int parentIno = getParentInode(&linkPath, &dummy);
    MINODE *pminode = iget(dev, parentIno);

    MINODE *mip = NULL;
    int ino = ialloc();
    char dbuf[BLKSIZE] = {0};

    mip = iget(dev, ino);
    INODE *inodeptr = &mip->INODE;
    inodeptr->i_mode = 0xA000; // OR 040755: DIR type and permissions
    inodeptr->i_uid = running->uid;     // Owner uid
    //ip->i_gid = running->gid;                   // Group Id
    inodeptr->i_size = BLKSIZE;   // Size in bytes
    inodeptr->i_links_count = 2;  // Links count=2 because of . and ..
    inodeptr->i_atime = time(0L); // set to current time
    inodeptr->i_ctime = time(0L);
    inodeptr->i_mtime = time(0L);
    inodeptr->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks
    inodeptr->i_size_high = 0;
    for (int i = 0; i < 15; i++)
    {
        inodeptr->i_block[i] = 0;
    }
    char path_buffer[BLKSIZE] = {0};
    strcpy(path_buffer, filePathString);
    int stlength = strlen(path_buffer) + 1;
    path_buffer[stlength] = 0;
    memcpy(&(inodeptr->i_block[0]), &path_buffer, stlength);

    //int parentIno = getParentInode(&linkPath, &dummy);
    //MINODE* pminode = iget(dev, parentIno);

    addDIR(pminode, ino, linkPath.baseName, 7);

    pminode->dirty = 1;
    iwrite(pminode);
    mip->dirty = 1;
    iwrite(mip);
}
int check_Dir_forSim_PRINTLINK(DIR *directory)
{
    MINODE *simlink;
    if (directory->file_type == 7)
    {
        simlink = iget(dev, directory->inode);
        printf("-->%s\n", simlink->INODE.i_block);
    }
    printf("\n");
}

void readSimLink(INODE *link)
{
    char linknamebuf[BLKSIZE] = {0};
    memcpy(&linknamebuf, &link->i_block[0], 32);

    Path linkfilePath;
    parseFilepath(linknamebuf, &linkfilePath);
    int linkInodenum = findInodeIndex(&linkfilePath, 7);

    INODE *linkInodeptr;
    getInode(linkInodenum, linkInodeptr);
    return linkInodeptr;
}

int returnSimLinkInumber(INODE *link)
{
    char linknamebuf[BLKSIZE] = {0};
    strcpy(link->i_block, linknamebuf);

    Path linkfilePath;
    parseFilepath(linknamebuf, &linkfilePath);
    int linkInodenum = findInodeIndex(&linkfilePath, 7);
    return linkInodenum;
}

int simlinkfromino(int inumber)
{
    MINODE *linkMinode;

    linkMinode = iget(dev, inumber);
    char linkBuffer[BLKSIZE] = {0};
    memcpy(&linkBuffer, &(linkMinode->INODE.i_block[0]), 32);

    Path path;
    parseFilepath(linkBuffer, &path);
    int inum = findInodeIndex(&path, type_Any);

    return inum;
}