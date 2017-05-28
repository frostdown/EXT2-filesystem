
#include "../header/blockUtil.h"


int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, SEEK_SET);
  int size = read(fd, buf, BLKSIZE);

  if (size == 0) 
  {
    printf("could not read block!");
    return 0;
  }
}
int put_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, SEEK_SET);
  write(fd, buf, BLKSIZE);
}

int tst_bit(char *buf, int bit) //This code is explained at 2.7.1
{
  int i, j;
  i = bit/8; j=bit%8;
  if (buf[i] & (1 << j)) //this shifts the buf array over to int J and checks if bit is 0 or 1. 
     return 1;
  return 0;
}

int set_bit(char *buf, int bit)
{
  int i, j;
  i = bit/8; j=bit%8; 
  buf[i] |= (1 << j); //this shifts the buff array over to int J and sets to bit to 1
}

int clr_bit(char *buf, int bit)
{
  int i, j;
  i = bit/8; j=bit%8; 
  buf[i] &= ~(1 << j); // "~" operator flips the bit over to the opposate of what it was. 1->0  and 0->1
}




// load a filesystem into memory
void loadFilesystem(char* filesystem_file)
{
    dev = fd = open(filesystem_file, O_RDWR);
    if (fd < 0)
    {
        printf("open %s failed!\n", filesystem_file);
        exit(1);
    }
}


// check the super block magic number to ensure this is an EXT2 filesystem.
// this must be called after loadBlocks
int checkSuper()
{
    // check EXT2 magic number
    u16 magic = sp->s_magic;
    printf("magic = %x\n", magic);
    if (magic != 0xEF53)
    {
        printf("NOT an EXT2 Filesystem");
        return 0;
    }
    return 1;
}

// load super block and group descriptor into memory and print out important values
void loadBlocks()
{
    // load superblock into global memory
    get_block(fd, 1, superBuf);
    sp = (SUPER*)superBuf;

    ninoes = sp->s_inodes_count;
    printf("num inodes: %d\n", ninoes);

    nblocks = sp->s_blocks_count;
    printf("num blocks: %d\n", nblocks);

    // load group descriptor into global memory
    get_block(fd, 2, gdBuf);
    gp = (GD*)gdBuf;

    bmap = gp->bg_block_bitmap;
    printf("bmap at block: %d\n", bmap);

    imap = gp->bg_inode_bitmap;
    printf("imap at block: %d\n", imap);

    start_iblock = gp->bg_inode_table;
    printf("inode table begins at block: %d\n", start_iblock);
}

int deallocateblock(int blk)
{
  char block_buf[BLKSIZE] = {0};

  // read block bitmap
  get_block(dev, bmap, block_buf);

  clr_bit(block_buf, blk); 
  put_block(dev, bmap, block_buf);
}

int balloc()
{
  char temp_buf[BLKSIZE];

  // read block bitmap
  get_block(dev, bmap, temp_buf);

  for (int i = 0; i < nblocks; i++)
  {

    if (tst_bit(temp_buf, i) == 0) //find the first open block in the bmap
    {
      set_bit(temp_buf, i); //set that block to 1 to show that its being used
      put_block(dev, bmap, temp_buf);
      return i + 1;        //return the bock number 
    }
  }
  printf("FAILED NO BLOCK ALLOCATED!!!!!!!!\n");
  exit(1);
}


