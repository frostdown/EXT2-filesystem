#include "../header/getDir.h"


int getDir(int parentInum, SearchValue searchValue, SearchType searchType, FileType fileType, DIR* dir)
{
    // get parent inode
    INODE parent;
    getInode(parentInum, &parent);

    // search through each directory
    int i = 0;
    int currentBlock = 0;
    int result = 0;

    for (i = 0; i < 12; i++)
    {
        currentBlock = parent.i_block[i];
        if (currentBlock == 0)
        {
            printf("dir not found!\n");
            return 1;
        }

        result = getDirInIndirectBlock(currentBlock, searchValue, searchType, fileType, 0, dir);

        if (result == 0)
        {
            return 0;
        }
    }

    result = getDirInIndirectBlock(parent.i_block[12], searchValue, searchType, fileType, 1, dir);
    if (result == 0) return result;
    result = getDirInIndirectBlock(parent.i_block[13], searchValue, searchType, fileType, 2, dir);
    if (result == 0) return result;
    result = getDirInIndirectBlock(parent.i_block[14], searchValue, searchType, fileType, 3, dir);
    if (result == 0) return result;

    return 1;
}

int getDirInIndirectBlock(int blockNum, SearchValue searchValue, SearchType searchType, FileType fileType, int numIndirections, DIR* dir)
{
    // valid block?
    if (blockNum == 0) 
        return 1;

    char block[BLKSIZE];
    get_block(fd, blockNum, block);

    // no indirection
    if (numIndirections == 0) 
    {
        return getDirInBlock(block, searchValue, searchType, fileType, dir);
    }

    // one or more indirections
    int i = 0;
    int* indirection = &block;
    int result = 0;

    for (i = 0; i < 256; i++)
    {
         result = getDirInIndirectBlock(indirection[i], searchValue, searchType, fileType, numIndirections-1, dir);

         if (result == 0)
         {
            return 0;
         }
    }

    return 1;
}

int getDirInBlock(char* blockBuf, SearchValue searchValue, SearchType searchType, FileType fileType, DIR* result)
{
    char* cp = blockBuf;
    DIR* dp = (DIR*)blockBuf;
    char name[128];

    while (cp < &blockBuf[BLKSIZE])
    {
        memcpy(name, dp->name, dp->name_len);
        name[dp->name_len] = '\0';
        
        switch (searchType)
        {
            case Search_Name:
                if (strcmp(searchValue.name, name) == 0)
                {
                    if (dp->file_type == (int)fileType || fileType == type_Any)
                    {
                        // return the found DIR
                        *result = *dp;
                        return 0;
                    }
                    else
                    {
                        printf("ERROR: file is not the desired type!\n");
                        return 1;
                    }
                }
                break;
            case Search_INum:
                if (dp->inode == searchValue.inumber)
                {
                    if (dp->file_type == (int)fileType || fileType == type_Any)
                    {
                        *result = *dp;
                        return 0;
                    }
                    else
                    {
                        printf("ERROR: file is not the desired type!\n");
                        return 1;
                    }
                }
                break;
        }

        cp += dp->rec_len;
        dp = (DIR*)cp;
    }

    return 1;
}

int getParentDir(int inumber, DIR* result)
{
    SearchValue sv;
    sv.name = "..";
    return getDir(inumber, sv, Search_Name, type_Directory, result);
}

