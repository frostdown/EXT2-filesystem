#ifndef SIMLINK_H
#define SIMLINK_H

#include "../globals.h"
#include "../parser.h"
#include "../dirUtil.h"
#include "../inodeUtil.h"
#include "../blockUtil.h"
#include "../search.h"

int simlink(Command* command);
int check_Ino_forSim(int ino);
int check_Dir_forSim_PRINTLINK(DIR *directory);
void readSimLink(INODE *link);
int simlinkfromino(int inumber);
#endif