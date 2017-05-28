#ifndef GET_DIR_H
#define GET_DIR_H

#include "dataStructures.h"
#include "blockUtil.h"
#include "inodeUtil.h"
#include "globals.h"

// directory utility
int getDir(int parentInum, SearchValue searchValue, SearchType searchType, FileType fileType, DIR* dir);
int getParentDir(int inumber, DIR* result);

#endif
