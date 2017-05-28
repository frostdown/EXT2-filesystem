#ifndef MKDIR_H
#define MKDIR_H

#include "../globals.h"
#include "../search.h"
#include "../inodeUtil.h"
#include "../blockUtil.h"




int mkDir(Command* command);
int mymkdir(MINODE *pip, char *name);
int my_creat(MINODE *pip, char *name);
int creat_file(Command* command);




#endif