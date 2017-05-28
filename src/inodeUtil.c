
#include "../header/inodeUtil.h"

// break a filepath into the individual files
void parseFilepath(char *filepath, Path *result)
{
    char *s = NULL;
    char currentPath[128];
   memset(result, 0, sizeof(Path));
    result->pathType = EmptyPath;

    // case for empty filepath
    if (strlen(filepath) == 0)
        return;

    // check filepath type
    if (*filepath == '/')
    {
        result->pathType = AbsolutePath;
        strcpy(currentPath, filepath + 1);
    }
    else
    {
        result->pathType = RelativePath;
        strcpy(currentPath, filepath);
    }

    // check if there is only one token in the path
    if (strstr(currentPath, "/") == NULL)
    {
        strcpy(result->baseName, currentPath);
        return;
    }

    int i = 0;
    s = strtok(currentPath, "/");
    strcpy(result->tokenizedPath[i], s);
    i++;

    // copy directories in path
    while (s = strtok(0, "/"))
    {
        strcpy(result->tokenizedPath[i], s);
        i++;
    }

    if (s != NULL)
    {
        // the base is left in s, move to base name
        strcpy(result->baseName, s);
    }
    else
    {
        // the base was moved to pathName, move to base name.
        strcpy(result->baseName, result->tokenizedPath[i - 1]);
        strcpy(result->tokenizedPath[i - 1], "");
    }

    // ensure the last entry in path is an empty string
    strcpy(result->tokenizedPath[i - 1], "");
}

void getInode(int index, INODE *result)
{
    // get block
    int iblock = (index - 1) / 8;
    int istart = start_iblock;
    get_block(fd, istart + iblock, buf);

    // get INODE
    int inum = (index - 1) % 8;
    getInodeFromBlock(buf, inum, result);
}

void putInode(int index, INODE *inode)
{
    // get block
    int iblock = (index - 1) / 8;
    int istart = start_iblock;
    get_block(fd, istart + iblock, buf);

    // put inode in place
    int inum = (index - 1) % 8;
    memcpy((INODE *)buf + inum, inode, sizeof(INODE));

    put_block(fd, iblock, buf);
}

void getInodeFromBlock(char *iblock, int index, INODE *result)
{
    // copy INODE from block into structure
    memcpy(result, (INODE *)iblock + index, sizeof(INODE));
}

void getRootInode(INODE *result)
{
    // root INODE is always 2
    // get INODE 2
    getInode(2, result);
}

int findFileInode(INODE *parent, char *filename, FileType type, INODE *result)
{
    // get first block in parent
    INODE *temp;
    if(parent->i_mode == 0xA000)
    {
       temp = readSimLink(parent);
    }
    else
    {
        temp = parent;
    }

    int currentBlock = temp->i_block[0];
    int index = 0;
    int i = 0;

    for (i = 0; i < 12; i++)
    {
        currentBlock = temp->i_block[i];
        if (currentBlock == 0)
        {
            printf("file not found!\n");
            return 0;
        }

        get_block(fd, currentBlock, buf);

        index = findInodeInBlock(buf, filename, type, result);

        if (index != 0)
        {   
            return index;
        }
    }

    printf("file not found!!!\n");
    return 0;

    // TODO indirect blocks
}

// returns the index of the INODE in a block
int findInodeIndexInBlock(char *blockBuf, char *filename, FileType type)
{
    // search a block
    char *cp = blockBuf;
    DIR *dp = (DIR *)blockBuf;
    char name[128];
    int inum = 0;

    while (cp < &blockBuf[BLKSIZE])
    {
        memcpy(name, dp->name, dp->name_len);
        name[dp->name_len] = '\0';

        if (strcmp(filename, name) == 0)
        {
            if(dp->file_type == 7)
            {
                inum = simlinkfromino(dp->inode);
                return inum;
            }
            if (type == type_Any || dp->file_type == (int)type)
            {
                // get the INODE of this file
                inum = dp->inode;
                return inum;
            }
            else
            {
                printf("ERROR: file is not the desired type!\n");
                return 0;
            }
        }

        cp += dp->rec_len;
        dp = (DIR *)cp;
    }

    return 0;
}

int findInodeInBlock(char *blockBuf, char *filename, FileType type, INODE *result)
{
    int index = findInodeIndexInBlock(blockBuf, filename, type);
    if (index != 0)
        getInode(index, result);
    return index;
}

int findInodePath(INODE *startingNode, Path *path, INODE *result, FileType endType)
{
    // search for the desired node
    int i = 0;
    INODE currentNode = *startingNode;
    INODE nextNode;
    int findResult = 0;

    for (i = 0; strcmp(path->tokenizedPath[i], "") != 0; i++)
    {
        char *dirname = path->tokenizedPath[i];
        findResult = findFileInode(&currentNode, dirname, type_Directory, &nextNode);

        if (findResult == 0)
        {
            printf("file not found!");
            return 0;
        }

        currentNode = nextNode;
    }

    // get the last INODE
    return findFileInode(&currentNode, path->baseName, endType, result);
}

int findInodeAbsPath(Path *filepath, INODE *result, FileType endType)
{
    // check path is empty
    if (strcmp(filepath->baseName, "") == 0)
    {
        if (endType == type_File)
        {
            return 0;
        }

        getRootInode(result);
        return 2;
    }

    // search for the desired inode
    INODE root;
    getRootInode(&root); //this sometimes does not grab the root inode!

    return findInodePath(&root, filepath, result, endType);
}

int findInode(Path *filepath, FileType endType, INODE *result)
{
    // absolute or relative?
    if (filepath->pathType == AbsolutePath)
    {
        // return absolute path result
        return findInodeAbsPath(filepath, result, endType);
    }
    else
    {
        // return relative path result
        return findInodePath(&(running->cwd->INODE), filepath, result, endType);
    }
}

int findInodeIndex(Path *filepath, FileType endType)
{
    INODE dummyINODE;
    // absolute or relative?
    if (filepath->pathType == AbsolutePath)
    {
        // return absolute path result
        return findInodeAbsPath(filepath, &dummyINODE, endType);
    }
    else
    {
        // return relative path result
        return findInodePath(&(running->cwd->INODE), filepath, &dummyINODE, endType);
    }
}

int decFreeInodes()
{
    char temp_buf[BLKSIZE] = {0};

    // dec free INODEs count in SUPER and GD
    get_block(dev, 1, temp_buf);
    sp = (SUPER *)temp_buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, temp_buf);

    get_block(dev, 2, temp_buf);
    gp = (GD *)temp_buf;
    gp->bg_free_inodes_count--;
    put_block(dev, 2, temp_buf);
}

int incFreeInodes()
{
    char buf[BLKSIZE];

    // dec free INODEs count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}

int ialloc()
{
    char temp_buf[BLKSIZE] = {0};

    // read INODE_bitmap block
    get_block(dev, imap, temp_buf);

    for (int i = 0; i < NMINODE; i++)
    {
        if (tst_bit(temp_buf, i) == 0)
        {
            set_bit(temp_buf, i);
            decFreeInodes();

            put_block(dev, imap, temp_buf);

            return i + 1;
        }
    }
    printf("ialloc(): no more free INODEs\n");
    return 0;
}

int deialloc(int inode)
{
    char inode_buf[BLKSIZE];

    // read INODE_bitmap block
    get_block(dev, imap, inode_buf);

    clr_bit(inode_buf, (inode-1));
    incFreeInodes();

    put_block(dev, imap, inode_buf);

}

void truncatePath(Path *path)
{
    if (strcmp(path->tokenizedPath[0], "") == 0)
    {
        if (path->pathType == AbsolutePath)
        {
            strcpy(path->baseName, "");
        }
        else
        {
            strcpy(path->baseName, ".");
        }
        return;
    }

    // find the end of the tokenizedPath
    int i = 0;
    while (strcmp(path->tokenizedPath[i + 1], "") != 0)
        i++;
    // i is now the last index in tokenizedPath

    // move i index to baseName
    strcpy(path->baseName, path->tokenizedPath[i]);
    strcpy(path->tokenizedPath[i], "");
}

int getParentInode(Path *path, INODE *inode)
{
    Path tempPath = *path;

    // truncate the base name off the end of the path
    truncatePath(&tempPath);

    // get the parent inode
    int result = findInode(&tempPath, type_Directory, inode);

    if (result == 0)
    {
        printf("failed to get parent inode!");
        exit(1);
    }

    return result;
}
