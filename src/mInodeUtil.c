#include "../header/mInodeUtil.h"

MINODE *iget(int dev, int ino)
{
    int i, blk, disp;
    char dbuf[BLKSIZE];
    MINODE *mip;
    INODE *ip;
    for (i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->refCount != 0 && mip->dev == dev && mip->ino == ino)
        {
            mip->refCount++;
            //printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
            return mip;
        }
    }
    for (i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->refCount == 0)
        {
            //printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
            mip->refCount = 1;
            mip->dev = dev;
            mip->ino = ino; // assing to (dev, ino)
            mip->dirty = mip->mounted = mip->mptr = 0;
            // get INODE of ino into dbuf[ ]
            blk = (ino - 1) / 8 + start_iblock; // iblock = Inodes start block #
            disp = (ino - 1) % 8;
            //printf("iget: ino=%d blk=%d disp=%d\n", ino, blk, disp);
            get_block(dev, blk, dbuf);
            ip = (INODE *)dbuf + disp;
            // copy INODE to mp->INODE
            mip->INODE = *ip;
            return mip;
        }
    }
    printf("PANIC: no more free minodes\n");
    return 0;
}

// remove a reference to an minode
int iput(MINODE *mip)
{
    if (mip->refCount > 0)
    {
        mip->refCount--;

        if (mip->refCount == 0)
        {
            iwrite(mip);
        }
    }
}

// write an minode back to disk
// WARNING: does not care about the minode ref count
// TODO modify to care about where this was mounted
int iwrite(MINODE *mip)
{
    int blk = 0;
    int offset = 0;

    if (mip->dirty)
    {
        // find block number
        int blockNum = mip->ino;

        blk = (((blockNum)-1) / 8) + start_iblock;
        offset = ((blockNum)-1) % 8;

        char inodeBlock[BLKSIZE];
        get_block(dev, blk, inodeBlock);

        ip = (INODE *)inodeBlock + offset;
        *ip = mip->INODE;
        // write back to that block number
        put_block(fd, blk, inodeBlock);
    }
}

// remove all data blocks from an minode
int truncate(MINODE* mip)
{
    // deallocate inode data blocks
    deallocateBlocks(&mip->INODE);

    // update inodes time field
    unsigned int now = (unsigned)time(NULL);
    mip->INODE.i_mtime = now;

    // set inode size to 0
    mip->INODE.i_size = 0;

    // mark mip dirty
    mip->dirty = 1;
}

int deallocateBlocks(INODE* inode)
{
    for (int i = 0; i < 12; i++)
    {
        if (inode->i_block[i] == 0)
            return 0;
        
        deallocateBlocksRecursive(inode->i_block[i], 0);
        inode->i_block[i] = 0;
    }

    if (inode->i_block[12] == 0) return 0;
    deallocateBlocksRecursive(inode->i_block[12], 1);
    inode->i_block[12] = 0;
    if (inode->i_block[13] == 0) return 0;
    deallocateBlocksRecursive(inode->i_block[13], 2);
    inode->i_block[13] = 0;
    if (inode->i_block[14] == 0) return 0;
    deallocateBlocksRecursive(inode->i_block[14], 3);
    inode->i_block[14] = 0;

}

int deallocateBlocksRecursive(int blockNum, int numIndirections)
{
    // check if block number is invalid
    if (blockNum == 0) return 0;

    // no indirections
    if (numIndirections == 0)
    {
        return deallocateblock(blockNum);
    }

    // one or more indirection
    int i = 0;
    char block[BLKSIZE];
    get_block(fd, blockNum, block);
    int* indirection = &block;

    for (i = 0; i < 256; i++)
    {
        printIndirectDirectory(indirection[i], numIndirections-1);
    }

    deallocateblock(blockNum);
}





