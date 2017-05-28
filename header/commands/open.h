#ifndef OPEN_H
#define OPEN_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../dataStructures.h"
#include "../globals.h"
#include "../inodeUtil.h"
#include "../blockUtil.h"

int myOpen(Command* command);
int openhelper(int inum, int openMode);


#endif
