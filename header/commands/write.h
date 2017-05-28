#ifndef WRITE_H
#define WRITE_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../dataStructures.h"
#include "../globals.h"
#include "../inodeUtil.h"
#include "../blockUtil.h"

int write_double_indirect(int *iblock_array, MINODE *mip);
int write_indirect(int *iblock_array, MINODE *mip);
int write_file(Command* command);
int mywrite(int fd, char writebuf[], int bytes);


#endif
