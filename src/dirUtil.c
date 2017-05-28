#include "../header/dirUtil.h"

int addDIR(MINODE *pip, int ino, char *dirName, int filetype)
{
    DIR newdir;
    DIR *dirptr = NULL;

    char dbuf[BLKSIZE] = {0}; // buffer for directory
    char sbuf[BLKSIZE] = {0}; //buffer for string compare

    char *current_block_position = NULL;

    int ideal_len = 0, need_len = 0, remain_space = 0, blk = 0, block_number = 0, x = 0;

    int newdirnamelength = strlen(dirName);

    while (pip->INODE.i_block[x] != 0)
    {
        block_number = pip->INODE.i_block[x];
        x++;
    }

    get_block(pip->dev, block_number, dbuf);

    dirptr = (DIR *)dbuf;
    current_block_position = dbuf;

    while (current_block_position + dirptr->rec_len < dbuf + BLKSIZE)
    {
        current_block_position += dirptr->rec_len;
        dirptr = (DIR *)current_block_position;
    }

    ideal_len = 4 * ((8 + dirptr->name_len + 3) / 4); //find the ideal length of the last dir entry
    remain_space = dirptr->rec_len - ideal_len;

    need_len = 4 * ((8 + newdirnamelength + 3) / 4);

    if (remain_space >= need_len) //enough space to add new dir to end of block
    {
        printf("ideal length:%d\n", ideal_len);
        printf("remaining space:%d\n", remain_space);
        printf("address to start write:%x\n", current_block_position);

        dirptr->rec_len = ideal_len; //truncate last entry to make room for new entry

        current_block_position += dirptr->rec_len;

        newdir.inode = (__u32 *)ino;
        newdir.file_type = filetype;
        newdir.rec_len = (__u16 *)remain_space;
        newdir.name_len = (__u8 *)newdirnamelength;
        memcpy(newdir.name, dirName, newdirnamelength);

        memcpy(current_block_position, &newdir, need_len);

        put_block(dev, block_number, dbuf);
        return 1;
    }
    else //need to make a new block for dir also increment parent size by 1024
    {
        printf("making new block for dir entry/n");
        int newblock = balloc();

        pip->INODE.i_block[x] = newblock;
        block_number = newblock;

        get_block(dev, block_number, dbuf);
        current_block_position = dbuf;
        dirptr = (DIR *)dbuf;

        newdir.inode = (__u32 *)ino;
        newdir.rec_len = (__u16 *)BLKSIZE;
        newdir.file_type = filetype;
        newdir.name_len = (__u8 *)strlen(dirName);
        need_len = 4 * ((8 + newdir.name_len + 3) / 4);
        memcpy(newdir.name, dirName, newdirnamelength);

        *dirptr = newdir;

        put_block(dev, newblock, dbuf);

        pip->INODE.i_size += 1024;
        iwrite(pip);
        return 1;
    }
    iwrite(pip);
    return 0;
}

int findBlockWDir(int parentInum, SearchValue sv, SearchType st)
{
    // get the parent inum
    MINODE *parent;
    parent = iget(dev, parentInum);

    // find which block contains the dir
    int i = 0;
    int currentBlock = 0;
    int result = 0;

    DIR dummy;

    for (i = 0; i < 12; i++)
    {
        currentBlock = parent->INODE.i_block[i];
        if (currentBlock == 0)
        {
            return 0;
        }

        result = getDirInIndirectBlock(currentBlock, sv, st, type_Any, 0, &dummy);

        if (result == 0)
        {
            return currentBlock;
        }
    }

    result = findBlockWDirInIndirectBlock(parent->INODE.i_block[12], sv, st, 1);
    if (result != 0)
        return result;
    result = findBlockWDirInIndirectBlock(parent->INODE.i_block[13], sv, st, 2);
    if (result != 0)
        return result;
    result = findBlockWDirInIndirectBlock(parent->INODE.i_block[14], sv, st, 3);
    if (result != 0)
        return result;

    return 0;
}

int findBlockWDirInIndirectBlock(int blockNum, SearchValue sv, SearchType st, int numIndirections)
{
    // check block is valid
    if (blockNum == 0)
        return 0;

    char block[BLKSIZE];
    get_block(fd, blockNum, block);
    int result = 0;
    DIR dummy;

    // no indirection
    if (numIndirections == 0)
    {
        result = getDirInIndirectBlock(blockNum, sv, st, type_Any, 0, &dummy);
        if (result != 0)
            return blockNum;
    }

    int i = 0;
    int *indirection = &block;

    for (i = 0; i < 256; i++)
    {
        result = getDirInIndirectBlock(blockNum, sv, st, numIndirections - 1);
        if (result != 0)
            return result;
    }

    return 0;
}

DIR *getDirPointer(char *blockBuf, SearchValue sv, SearchType st, DIR **dirBefore)
{
    char *cp = blockBuf;
    DIR *dp = (DIR *)blockBuf;
    char name[128];

    while (cp < &blockBuf[BLKSIZE])
    {
        memcpy(name, dp->name, dp->name_len);
        name[dp->name_len] = '\0';

        switch (st)
        {
        case Search_Name:
            if (strcmp(sv.name, name) == 0)
            {
                // return the found DIR
                return dp;
            }
            break;
        case Search_INum:
            if (dp->inode == sv.inumber)
            {
                // return the found DIR
                return dp;
            }
            break;
        }

        cp += dp->rec_len;
        *dirBefore = dp;
        dp = (DIR *)cp;
    }

    return NULL;
}

int findDir(Path *path, FileType fileType, DIR *result)
{
    int callResult = 0;

    // get the parent dir
    INODE parent;
    callResult = getParentInode(path, &parent);

    if (callResult == 0)
        exit(0);

    // get the dir
    SearchValue sv;
    sv.name = path->baseName;
    callResult = findBlockWDir(callResult, sv, Search_Name);
    if (callResult == 0)
        exit(0);
    char block[BLKSIZE];
    get_block(fd, callResult, block);
    DIR *dummy;
    *result = *getDirPointer(block, sv, Search_Name, &dummy);
}

// remove the specified dir from its parent directory
// returns whether the dir was successfully removed or not.
int removeDIR(int parent, SearchValue sv, SearchType st)
{
    int result = 0;

    // find the block with the DIR we need to remove
    int resultBlock = 0;
    resultBlock = findBlockWDir(parent, sv, st);

    if (resultBlock == 0)
        return 0;

    // get the block
    char blockbuf[BLKSIZE] = {0};
    get_block(fd, resultBlock, blockbuf);

    // find the dir structure
    DIR *previousDir = NULL;
    DIR *deletingDir = getDirPointer(blockbuf, sv, st, &previousDir);

    // delete the dir structure
    char *currentDir = (char *)deletingDir;
    char *nextDir = currentDir + deletingDir->rec_len;
    memset(currentDir, 0, deletingDir->rec_len);

    // check if the dir we deleted was the only dir in the block
    if (previousDir == NULL)
    {
        // deallocate the block
        // TODO deallocate the block
    }

    // check if the dir we deleted was the last dir
    if (nextDir >= blockbuf + BLKSIZE)
    {
        // update the previous dir
        previousDir->rec_len = (int)(blockbuf + BLKSIZE - (char *)previousDir);
        put_block(fd, resultBlock, blockbuf);
        return 0;
    }

    __u16 recLen = 0;
    __u16 ideal_len = 0;
    // shuffle up the remaining dirs
    while (nextDir < blockbuf + BLKSIZE)
    {
        // copy down
        DIR *nextDIR = (DIR *)nextDir;
        DIR *currentDIR = (DIR *)currentDir;
        recLen = nextDIR->rec_len;
        ideal_len = 4 * ((8 + nextDIR->name_len + 3) / 4);

        memmove(currentDir, nextDir, recLen);

        // update next pointer
        nextDir += recLen;
        currentDir += recLen;
    }
    currentDir -= recLen;
    // currentDirectory should be pointing at the last directory in the block
    DIR *currentDIR = (DIR *)currentDir;
    currentDIR->rec_len = (int)(blockbuf + BLKSIZE - currentDir);

    // put the block back into it's place
    put_block(fd, resultBlock, blockbuf);
    return 0;
}
