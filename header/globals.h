#ifndef GLOBALS_H
#define GLOBALS_H

#include "dataStructures.h"


SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

#define BLKSIZE     1024

#define NMINODE      100
#define NFD           16
#define NPROC          4


// buffers
char superBuf[BLKSIZE];
char gdBuf[BLKSIZE];

// standard buffer 
char buf[BLKSIZE];

// minodes
MINODE minode[NMINODE]; // minode array
MINODE *root; // pointer to root minode
PROC proc[NPROC];
PROC* running; // pointer to the currently running proc structure

// file descriptors
int fd;
int dev;

// file system constants
int nblocks, ninoes, bmap, imap, start_iblock;
int ninodes, nfreeInodes, nfreeBlocks;

char* disk;
char line[128], cmd[64], *kpathname[16], kpath[64];



#endif
