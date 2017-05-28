#include "../../header/commands/write.h"

int write_file(Command *command)
{

    //grab the fd
    int writefd = atoi(command->tokenizedCommand[1]);

    //check file mode
    if (running->fd[writefd]->mode == 0)
    {
        printf("Error file in read only mode!\n");
        return 0;
    }

    // grab the string to write
    printf("Enter line to be written to file:");
    char *userstring = get_user_input();

    //check that fd is open in WR, RW, or APPEND mode

    //copy string to buff
    char stringbuf[BLKSIZE] = {0};
    strcpy(stringbuf, userstring);

    //get the stringlength as bytes
    int stringlength = 0;
    stringlength = strlen(stringbuf);
    return mywrite(writefd, stringbuf, stringlength);
}

int mywrite(int fd, char writebuf[], int bytes)
{
    int diskBlock = 0;
    int writtenbytes = 0;

    MINODE *mip = NULL;
    mip = running->fd[fd]->mptr;
    OFT *oftp = running->fd[fd];
    char setblocktozero[BLKSIZE] = {0};

    while (bytes > 0)
    {
        int logicalBlock = oftp->offset / BLKSIZE;
        int startByte = oftp->offset % BLKSIZE;

        if (logicalBlock < 12)
        {
            if (mip->INODE.i_block[logicalBlock] == 0) //changed from ip-> to mip->
            {
                mip->INODE.i_block[logicalBlock] = balloc();
                get_block(mip->dev, mip->INODE.i_block[logicalBlock], setblocktozero);
                memset(setblocktozero, 0, BLKSIZE);
                put_block(mip->dev, mip->INODE.i_block[logicalBlock], setblocktozero);
            }
            diskBlock = mip->INODE.i_block[logicalBlock];
        }
        else if (logicalBlock >= 12 && logicalBlock < (256 + 12))
        {
            if (mip->INODE.i_block[12] == 0)
            {
                mip->INODE.i_block[12] = balloc();
                get_block(mip->dev, mip->INODE.i_block[12], setblocktozero);
                memset(setblocktozero, 0, BLKSIZE);
                put_block(mip->dev, mip->INODE.i_block[12], setblocktozero);
            }

            char indirectbuf[BLKSIZE] = {0};
            get_block(mip->dev, mip->INODE.i_block[12], indirectbuf);
            int *indirect = (int *)indirectbuf;
            diskBlock = write_indirect(indirect, mip);
            put_block(mip->dev, 12, indirectbuf);
        }
        else
        {
            if (mip->INODE.i_block[13] == 0)
            {
                mip->INODE.i_block[13] = balloc();
                get_block(mip->dev, mip->INODE.i_block[13], setblocktozero);
                memset(setblocktozero, 0, BLKSIZE);
            }
            char doubleindirectbuf[BLKSIZE] = {0};
            get_block(mip->dev, mip->INODE.i_block[13], doubleindirectbuf);
            int *indirect = (int *)doubleindirectbuf;
            diskBlock = write_double_indirect(indirect, mip);
            put_block(mip->dev, logicalBlock, doubleindirectbuf);
        }
        /* all cases come to here : write to the data block */
        char wbuf[BLKSIZE] = {0};
        get_block(mip->dev, diskBlock, wbuf); // read disk block into wbuf[ ]
        char *cq = writebuf;
        char *cp = wbuf + startByte;      // cp points at startByte in wbuf[]
        int remain = BLKSIZE - startByte; // number of BYTEs remain in this block

        if ((bytes - remain) <= 0)
        {
            memcpy(cp, cq, bytes);
            writtenbytes += bytes;
            bytes = 0;
        }
        else
        {
            memcpy(cp, cq, remain);
            bytes -= remain;
            writtenbytes += remain;
        }

        running->fd[fd]->offset += writtenbytes; // advance offset
        if (oftp->offset > mip->INODE.i_size)    // especially for RW|APPEND mode
            mip->INODE.i_size += writtenbytes;   // inc file size (if offset > fileSize)

        put_block(mip->dev, diskBlock, wbuf); // write wbuf[ ] to disk

        // loop back to while to write more .... until nbytes are written
    }

    mip->dirty = 1; // mark mip dirty for iput()
    iwrite(mip);
    printf("wrote %d char into file descriptor fd=%d\n", writtenbytes, fd);
    return bytes;
}

int write_indirect(int *iblock_array, MINODE *mip)
{
    int block_number, ino;
    int diskblock = 0;
    int bob = 0;

    for (int i = 0; i < 256; i++)
    {
        block_number = iblock_array[i];
        if (block_number == 0)
        {
            iblock_array[i] = bob = balloc();
            printf("Indirect block number:%d\n", bob);
            printf("Indirect block number:%d\n", iblock_array[i]);
            return iblock_array[i];
        }
    }
    return diskblock;
}

int write_double_indirect(int *iblock_array, MINODE *mip)
{
    char Blockbuf[BLKSIZE];
    int *indirect;
    int diskBlock = 0;
    for (int i = 0; i < 256; i++)
    {
        get_block(dev, iblock_array[i], Blockbuf);
        indirect = (int *)Blockbuf;
        if (indirect == 0)
        {
            iblock_array[i] = balloc();
            diskBlock = write_indirect(indirect, mip);
        }
    }
    return diskBlock;
}

int indirectBLock(int logicalBLock)
{
}