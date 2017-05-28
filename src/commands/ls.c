
#include "../../header/commands/ls.h"

int lsmode = 0; // 0 for normal, 1 for -l mode.


int printMinodeDirectory(MINODE* directory)
{
    INODE* inode = &directory->INODE;

    int i = 0;
    for (i = 0; i < 12; i++)
    {
        printIndirectDirectory(inode->i_block[i], 0);
    }

    printIndirectDirectory(inode->i_block[12], 1);
    printIndirectDirectory(inode->i_block[13], 2);
    printIndirectDirectory(inode->i_block[14], 3);

    printf("\n");
}

int printIndirectDirectory(int blockNum, int numIndirections)
{
    // check if block number is invalid
    if (blockNum == 0) return 0;

    // no indirections
    if (numIndirections == 0)
    {
        return printDirectory(blockNum);
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
}

// print out all the names in a block
int printDirectory(int blockNum)
{
    // get the block
    char block[BLKSIZE];
    get_block(fd, blockNum, block);

    // iterate through the directory block
    char* currentPlace = block;
    DIR* currentDir = (DIR*) currentPlace;

    char name[128];
    char color[5];

    while (currentPlace < block + BLKSIZE)
    {
        // create the directory string
        memcpy(name, currentDir->name, currentDir->name_len);
        name[currentDir->name_len] = '\0';

        // print out the entry
        if (lsmode == 0)
        {
            // normal print mode
            printf("%s", name);
            check_Dir_forSim_PRINTLINK(currentDir);
        }
        else
        {
            // long print mode
            printDIR_Long(currentDir, name);
        }


        // increment our place
        currentPlace += currentDir->rec_len;
        currentDir = (DIR*) currentPlace;
    }
}

void printDIR_Long(DIR* dir, char* name)
{
    // get minode
    MINODE* minode = iget(fd, dir->inode);

    // get inode
    INODE* inode = &minode->INODE;


    // get permissions and filetype
    char permissionStr[17] = "----------------";
    char fullPermissionStr[17] = "xwrcwrcwr-------";

    int16_t mode = inode->i_mode;

    // get file accessibility
    int j = 0;
    for (int i = 8; i >= 0; i--)
    {
        if (mode & (1 << i))
        {
            permissionStr[j] = fullPermissionStr[j];
        }
        j++;
    }

    // get filetype
    if((mode & 0xF000) == 0x8000)
        permissionStr[0] = '-';
    if((mode & 0xF000) == 0x4000)
        permissionStr[0] = 'd';
    if((mode & 0xF000) == 0xA000)
        permissionStr[0] = 'l';

    // get uid
    int uid = inode->i_uid;

    // get group id
    int gid = inode->i_gid;

    // get size
    int size = inode->i_size;

    // get modified time
    time_t modifiedTime = inode->i_mtime;

    // get time string
    char formattedTime[128];
    strcpy(formattedTime, ctime(&modifiedTime));
    formattedTime[strlen(formattedTime)-1] = '\0';

    // print everything out.
    printf("%s %d %d %7d %s %s", permissionStr, uid, gid, size, formattedTime, name);

    // check for simlink
    check_Dir_forSim_PRINTLINK(dir);

    iput(minode);
}

int ls(Command* command)
{
    MINODE* node;
    int pathLoc = 1;
    lsmode = 0;

    // is -l flag present?
    if (command->tokenizedCommand[1][0] == '-')
    {
        lsmode = 1;
        pathLoc = 2;
    }

    // is path empty? if so print out the current directory
    char* pathString = command->tokenizedCommand[pathLoc];
    if (strcmp(pathString, "") == 0)
    {
        // print current working directory of the running process
        node = running->cwd;
        printMinodeDirectory(node);
        return 0;
    }

    // get the path from the command
    Path path;
    parseFilepath(pathString, &path);

    // get the minode of the desired path
    int inodeIndex = findInodeIndex(&path, type_Directory);
    MINODE* minode = iget(dev, inodeIndex);

    // print minode
    printMinodeDirectory(minode);

    // release minode
    iput(minode);
}



