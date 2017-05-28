#include "../../header/commands/mkdir.h"

int mkDir(Command *command)
{
    if (command->tokenizedCommand[1][0] == 0)
    {
        printf("Error no Directory name specified!\n");
        return 0;
    }
    MINODE *pip = NULL;
    int checkforName = 1;
    int pino = 0, i = 0;

    Path filePath;
    char *filePathString = command->tokenizedCommand[1];
    parseFilepath(filePathString, &filePath);

    char newDir[64] = {0};
    strcpy(newDir, filePath.baseName);
    checkforName = findInodeIndex(&filePath, type_Directory);
    if (checkforName != 0)
    {
        printf("Dirname already exitst!\n");
        return 0;
    }

    if (filePath.pathType == AbsolutePath)
    {
        if (filePath.tokenizedPath[0][0] == 0)
        {
            pip = iget(dev, 2);
            dev = root->dev;
        }
        else
        {
           /* //int i = 0;
            //while (filePath.tokenizedPath[i][0] != 0 && i < 10)
            //{
            //    i++;
            //}
            //i--;
            strcpy(filePath.baseName, filePath.tokenizedPath[i]);
            filePath.tokenizedPath[i][0] = 0;
            int inum = findInodeIndex(&filePath, type_Directory); */
            INODE dummy;
            int inum = getParentInode(&filePath, &dummy);
            pip = iget(dev, inum);
            dev = root->dev;
            i = 0;
        }
    }
    else
    {
        pip = running->cwd;
        dev = running->cwd->dev;
    }

    mymkdir(pip, newDir);

    pip->refCount++;
    pip->INODE.i_links_count++;
    pip->dirty = 1;
    pip->INODE.i_atime = time(0L);
    pip->INODE.i_mtime = time(0L);
    iwrite(pip);
}

int mymkdir(MINODE *pip, char *name)
{
    MINODE *mip = NULL;
    int ino = ialloc();
    int bno = balloc();
    char dbuf[BLKSIZE] = {0};

    mip = iget(dev, ino);
    INODE *inodeptr = &mip->INODE;
    inodeptr->i_mode = 040755;      // OR 040755: DIR type and permissions
    inodeptr->i_uid = running->uid; // Owner uid
    //ip->i_gid = running->gid;                   // Group Id
    inodeptr->i_size = BLKSIZE;   // Size in bytes
    inodeptr->i_links_count = 2;  // Links count=2 because of . and ..
    inodeptr->i_atime = time(0L); // set to current time
    inodeptr->i_ctime = time(0L);
    inodeptr->i_mtime = time(0L);
    inodeptr->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks
    inodeptr->i_block[0] = bno;
    inodeptr->i_size_high = 0;
    for (int i = 1; i < 15; i++)
    {
        inodeptr->i_block[i] = 0;
    }

    mip->dirty = 1; // mark mINODE dirty
    iwrite(mip);    // write INODE to disk

    DIR rootdir;
    rootdir.rec_len = (__u16 *)12;
    rootdir.name_len = (__u8 *)1;
    rootdir.inode = (__u32 *)ino;
    rootdir.file_type = EXT2_FT_DIR;
    rootdir.name[0] = '.';
    memcpy(dbuf, &rootdir, 9);

    DIR parentdir;
    parentdir.rec_len = (__u16 *)1012;
    parentdir.name_len = (__u8 *)2;
    parentdir.inode = (__u32 *)pip->ino;
    parentdir.file_type = EXT2_FT_DIR;
    parentdir.name[0] = '.';
    parentdir.name[1] = '.';
    memcpy(dbuf + 12, &parentdir, 10);

    put_block(dev, bno, dbuf);

    addDIR(pip, ino, name, EXT2_FT_DIR);
}

int creat_file(Command *command)
{
    if (command->tokenizedCommand[1][0] == 0)
    {
        printf("Error no Directory name specified!\n");
        return 0;
    }

    MINODE *mip = NULL;
    MINODE *pip = NULL;
    int pino = 0, i = 0;
    char *child = NULL;
    int checkforName = 1;

    Path filePath;
    char *filePathString = command->tokenizedCommand[1];
    parseFilepath(filePathString, &filePath);

    char newDir[64] = {0};
    strcpy(newDir, filePath.baseName);
    checkforName = findInodeIndex(&filePath, type_File);
    if (checkforName != 0)
    {
        printf("Filename already exitst!\n");
        MINODE *fileptr = iget(dev, checkforName);
        fileptr->INODE.i_mtime = time(0L);
        return 0;
    }

    if (filePath.pathType == AbsolutePath)
    {
        if (filePath.tokenizedPath[0][0] == 0)
        {
            pip = iget(dev, 2);
            dev = root->dev;
        }
        else
        {
            int i = 0;
            while (filePath.tokenizedPath[i][0] != 0 && i < 10)
            {
                i++;
            }
            i--;
            strcpy(filePath.baseName, filePath.tokenizedPath[i]);
            filePath.tokenizedPath[i][0] = 0;
            int inum = findInodeIndex(&filePath, type_Directory);
            pip = iget(dev, inum);
            dev = root->dev;
            i = 0;
        }
    }
    else
    {
        pip = running->cwd;
        dev = running->cwd->dev;
    }


    my_creat(pip, newDir);

    pip->refCount = 1;
    pip->dirty = 1;
    pip->INODE.i_atime = time(0L);
    pip->INODE.i_mtime = time(0L);
    iwrite(pip);
}

int my_creat(MINODE *pip, char *name)
{
    MINODE *mip = NULL;
    int ino = ialloc();
    char dbuf[BLKSIZE] = {0};

    mip = iget(dev, ino);
    ip = &mip->INODE;
    INODE *ip = &mip->INODE;
    ip->i_mode = 0100644;
    ip->i_uid = running->uid; // Owner uid
    //ip->i_gid = running->gid;                   // Group Id
    ip->i_size = 0;         // Size in bytes
    ip->i_links_count = 1;  // Links count=2 because of . and ..
    ip->i_atime = time(0L); // set to current time
    ip->i_ctime = time(0L);
    ip->i_mtime = time(0L);
    ip->i_blocks = 0; // LINUX: Blocks count in 512-byte chunks
    ip->i_size_high = 0;

    mip->dirty = 1; // mark mINODE dirty
    addDIR(pip, ino, name, EXT2_FT_REG_FILE);
    iput(mip); // write INODE to disk
}