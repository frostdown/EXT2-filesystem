#ifndef PWD_H
#define PWD_H

#include "../dataStructures.h"
#include "../globals.h"
#include "../inodeUtil.h"
#include "../blockUtil.h"
#include "../getDir.h"
#include "../search.h"

void printdirectory(int inumber, int iblocknumber);
void pwdrecursive(int device, int inumber);
int pwd(Command* command);




#endif
