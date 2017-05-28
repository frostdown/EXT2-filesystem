#ifndef MINODE_UTIL_H
#define MINODE_UTIL_H

#include "../header/dataStructures.h"
#include "../header/blockUtil.h"
#include "../header/inodeUtil.h"

MINODE *iget(int dev, int ino);
int iput(MINODE* mip);

int iwrite(MINODE* mip);

// remove data blocks
int truncate(MINODE* mip);


#endif
