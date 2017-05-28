#ifndef DATA_STRUCTURES
#define DATA_STRUCTURES

#include <stdbool.h>
#include <stddef.h>
#include <ext2fs/ext2_fs.h>

// File System Data Structures
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

typedef struct minode {
  INODE INODE;
  int dev, ino;
  int refCount;
  int dirty;
  int mounted;
  struct mntable *mptr;
}MINODE;

typedef struct oft{
  int  mode;
  int  refCount;
  MINODE *mptr;
  int  offset;
}OFT;

#define NFD           16
typedef struct proc{
  struct proc *next;
  int          pid;
  int          uid;
  MINODE      *cwd;
  OFT         *fd[NFD];
}PROC;



typedef enum pathtype
{
	EmptyPath,
	RelativePath,
	AbsolutePath
} PathType;

typedef struct path
{
    char tokenizedPath[10][64];
    char baseName[64];
    PathType pathType
} Path;

typedef enum fileType
{
    type_Directory = 2,
    type_Link = 7,
    type_File = 1,
    type_Any = 42
} FileType;


typedef enum searchType
{
    Search_Name,
    Search_INum
}SearchType;

typedef union searchValue
{
    int inumber;
    char* name;
}SearchValue;



// Parser Data Structures
typedef struct ioRedirect
{
    int haveRedirect;
    int isAppend;
    char filename[128];
} IORedirect;

typedef struct command
{
    char tokenizedCommand[10][64];
    IORedirect inRedirect;
    IORedirect outRedirect;
} Command;

typedef struct pipe Pipe;

typedef struct pipe
{
    Command command;
    Pipe* leftPipe;
} Pipe;



Pipe* allocPipe();

void freePipe(Pipe* pipe);


#endif



