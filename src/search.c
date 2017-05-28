
#include "../header/search.h"

int search(int *dev, int searchnode)
{
    char dbuf[BLKSIZE]; //buffer to load root dir
    char sbuf[BLKSIZE]; //buffer to for loading dir names for use in strcmp
    int ino, block_number, offset;
    int *indirect;
    if (searchnode != 0)
    {
        block_number = (((searchnode)-1) / 8) + start_iblock;
        offset = ((searchnode)-1) % 8;
    }
    else
    {
        block_number = start_iblock;
        offset = 1;
    }
    get_block(dev, block_number, buf);
    ip = (INODE *)buf + offset;
    // Add one giving us the root INODE

    for (int x = 0; x < 12; x++)
    {
        block_number = ip->i_block[x];
        if (block_number == 0)
        {
            break;
        }
        ino = search_block(block_number);
    }
    if (ip->i_block[12] != 0)
    {

        get_block(dev, block_number, dbuf);
        indirect = dbuf;
        ino = search_indirect(indirect);
    }
    if (ip->i_block[13] != 0)
    {

        get_block(dev, block_number, dbuf);
        indirect = dbuf;
        ino = search_double_indirect(indirect, dev);
    }

    return ino;
}

int search_block(int searchblk)
{
    char dbuf[BLKSIZE];
    char buf[BLKSIZE];
    char sbuf[BLKSIZE];
    DIR *dp;
    char *cp;
    int blk, offset, x = 0;
    get_block(dev, searchblk, dbuf);

    cp = dbuf;
    dp = (DIR *)dbuf;

    for (int i = 0; kpathname[i] != NULL; i++)
    {
        while (cp < &dbuf[BLKSIZE]) //while cp address is less than dbuff max address
        {
            while (x < dp->name_len)
            {
                sbuf[x] = dp->name[x];
                x++;
            }
            sbuf[x] = NULL;
            x = 0;

            if (strcmp(kpathname[i], sbuf) == 0)
            {
                if (dp->file_type == 2)
                {

                    blk = (((dp->inode) - 1) / 8) + searchblk;
                    offset = ((dp->inode) - 1) % 8;

                    get_block(fd, blk, buf);
                    ip = (INODE *)buf + offset;
                    break;
                }
                else
                {
                    return dp->inode;
                }
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
            if (cp >= &dbuf[BLKSIZE])
            {
                return 0;
            }
        }
    }
    return dp->inode;
}

int search_indirect(int *iblock_array)
{
    int block_number, ino;

    for (int i = 0; i < 256; i++)
    {
        block_number = iblock_array[i];
        if (block_number == 0)
        {
            break;
        }
        ino = search_block(block_number);
    }
    return ino;
}

int search_double_indirect(int *iblock_array, int *dev)
{
    char dbuf[BLKSIZE];
    int *indirect;
    int ino;
    for (int i = 0; i < 256; i++)
    {
        get_block(dev, iblock_array[i], dbuf);
        indirect = dbuf;
        if (indirect == 0)
        {
            break;
        }
        ino = search_indirect(indirect);
    }
    return ino;
}

void tokenize_path(char *input)
{
    int i = 0;
    char *temp;

    temp = strtok(input, "/");

    while (temp != NULL)
    {
        kpathname[i] = temp;
        temp = strtok(NULL, "/");
        i++;
    }
    kpathname[i] = '\0';
}
void tokenize_cmd(char *input, char *tpath[])
{
    int i = 0;
    char *temp = NULL;

    printf("%s\n", input);
    temp = strtok(input, " ");

    while (temp != NULL)
    {
        tpath[i] = temp;
        temp = strtok(NULL, " ");
        i++;
    }
    tpath[i] = '\0';
}

char *get_user_input(void)
{
    char *buffer;
    size_t bufsize = 32;
    size_t characters;
    buffer = (char *)malloc(bufsize * sizeof(char));
    if (buffer == NULL)
    {
        perror("Unable to allocate buffer\n");
        exit(1);
    }

    characters = getline(&buffer, &bufsize, stdin);

    if (buffer[characters - 1] == '\n')
    {
        buffer[characters - 1] = '\0';
        characters--;
    }

    return buffer;
}

int get_str_length(char *input)
{
    int return_value = 0;

    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] == ' ' || input[i] == '/')
        {
            return_value++;
        }
    }
    return return_value;
}

void kpathnamehelper(char const *input)
{
    char *dummypath[10];
    char temp[64];
    char *temp2;
    strcpy(temp, input);
    tokenize_cmd(temp, dummypath);
    strcpy(cmd, dummypath[0]);

    if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0)
    {
          MINODE *ptr;
        for (int i = 0; minode[i].ino != 0; i++)
        {
            ptr = &(minode[i]);
            ptr->refCount = 1;
            iput(ptr); 
        }
        exit(1);
    }

    if (dummypath[1] != NULL)
    {
        strcpy(kpath, dummypath[1]);
        tokenize_path(dummypath[1]);
        return;
    }

}
