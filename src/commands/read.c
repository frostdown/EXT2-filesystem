
#include "../../header/commands/read.h"



int myRead(Command* command)
{
    // get fd an number of bytes to read
    int fileDescriptor = atoi(command->tokenizedCommand[1]);
    int numBytes = atoi(command->tokenizedCommand[2]);

    // verify fd is opened for RD or RW
    int mode = running->fd[fileDescriptor]->mode;
    if (mode != 0 && mode != 2)
    {
        printf("file not opened for read!");
        return -1;
    }
    
    return readFile(fileDescriptor, buf, numBytes);
}


int readFile(int fileDescriptor, char* buffer, int nbytes)
{
    int count = 0;
    OFT* oftp = running->fd[fileDescriptor];
    int offset = oftp->offset;
    int fileSize = oftp->mptr->INODE.i_size;
    int available = fileSize - offset;
    int bytesRead = 0;

    char* cq = buffer;

    while (nbytes > 0 && available > 0)
    {
        offset = oftp->offset;

        int logicalBlock = offset / BLKSIZE;
        int startByte = offset % BLKSIZE;

        int currentRead = readFileLogical(logicalBlock, fileDescriptor, cq, startByte, nbytes, available);
        nbytes -= currentRead;
        available -= currentRead;
        oftp->offset += currentRead;
        cq += currentRead;
        bytesRead += currentRead;
    }

    return bytesRead;
}

int readFileLogical(int logicalBlock, int fileDescriptor, char* buffer, int startByte, int nbytes, int avail)
{
    INODE* inode = &running->fd[fileDescriptor]->mptr->INODE;
    // logical block in direct blocks
    if (logicalBlock < 12)
    {
        return readBlockIndirect(0, inode->i_block[logicalBlock], buffer, logicalBlock, startByte, nbytes, avail);
    }

    // logical block in indirect block 1
    logicalBlock -= 12;
    if (logicalBlock < 256)
    {
        return readBlockIndirect(1, inode->i_block[12], buffer, logicalBlock, startByte, nbytes, avail);
    }

    // logical block in indirect block 2
    logicalBlock -= 256;
    if (logicalBlock < 256 * 256)
    {
        return readBlockIndirect(2, inode->i_block[13], buffer, logicalBlock, startByte, nbytes, avail);
    }

    // logical block in indirect block 3
    logicalBlock -= 256 * 256;
    return readBlockIndirect(3, inode->i_block[14], buffer, logicalBlock, startByte, nbytes, avail);
}


int readBlockIndirect(int numIndirects, int blockNum, char* buffer, int logicalBlock, int startByte, int nbytes, int avail)
{
    if (numIndirects == 0)
    {
        // no indirections, read block
        return readBlock(blockNum, buffer, startByte, nbytes, avail);
    }

    // figure out which index the block is in
    int weightPerEntry = myPow(256, numIndirects);

    int nextBlockLocation = logicalBlock / weightPerEntry;
    int newLogicalBlock = logicalBlock % weightPerEntry;

    char block[BLKSIZE];
    get_block(fd, blockNum, block);
    int* indirection = &block;

    int nextBlock = indirection[nextBlockLocation];

    return readBlockIndirect(numIndirects-1, nextBlock, buffer, newLogicalBlock, startByte, nbytes, avail);
}

int myPow(int base, int power)
{
    int result = 1;

    for (int i = 0; i < power; i++)
    {
        result *= base;
    }

    return result;
}



int readBlock(int block, char* buffer, int startByte, int nbytes, int avail)
{
    int remaining = BLKSIZE - startByte;
    
    int reading = myMin(remaining, nbytes);

    char readBlock[BLKSIZE];
    get_block(fd, block, readBlock);

    char *cp = readBlock + startByte;

    memcpy(buffer, cp, reading);

    return reading;
}

int myMin(int num1, int num2)
{
    if (num1 < num2)
    {
        return num1;
    }
    else
    {
        return num2;
    }
}


