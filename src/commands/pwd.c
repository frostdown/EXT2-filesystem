#include "../../header/commands/pwd.h"

void printdirectory(int child, int parentinode)
{
    int blk = (((parentinode)-1) / 8) + start_iblock;
    int offset = ((parentinode)-1) % 8;
    char sbuf[BLKSIZE] = {0};
    char dbuf[BLKSIZE] = {0};
    int iblocknumber = 0;
    get_block(dev, blk, dbuf);
    ip = (INODE *)dbuf + offset;

    for (int x = 0; x < 12; x++)
    {
        iblocknumber = ip->i_block[x];
        if (iblocknumber == 0)
        {
            break;
        }
        get_block(dev, iblocknumber, dbuf);

        char *cp = dbuf;
        DIR *dp = (DIR *)dbuf;

        while (cp < &dbuf[BLKSIZE]) //while cp address is less than dbuff max address
        {
            if (dp->inode == child)
            {
                while (x < dp->name_len)
                {
                    sbuf[x] = dp->name[x];
                    x++;
                }
                sbuf[x] = 0;
                printf("%s", sbuf);
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
            if (cp >= &dbuf[BLKSIZE])
            {
                return 0;
            }
        }
    }
}

void pwdrecursive(int device, int inumber)
{
    char tempbuf[BLKSIZE] = {0};
    if (inumber == 2)
    {
        return;
    }
    int iblocknumber = 0;
    int offset = ((inumber)-1) % 8;
    int blk = (((inumber)-1) / 8) + start_iblock;

    get_block(dev, blk, tempbuf);

    ip = (INODE *)tempbuf + offset;

    blk = ip->i_block[0];

    iblocknumber = search_block(blk);

    pwdrecursive(dev, iblocknumber); //change to look at current device for level 3
    printf("/");
    printdirectory(inumber, iblocknumber);
}

int pwd(Command *command)
{
    if (running->cwd->ino == 2)
    {
        printf("/\n");
        return 0;
    }
    strcpy(kpath, "..");
    kpathname[0] = "..";
    kpathname[1] = 0;
    pwdrecursive(dev, running->cwd->ino);
    printf("\n");

    return 0;
}
