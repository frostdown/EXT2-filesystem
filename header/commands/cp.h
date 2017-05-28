#ifndef CP_H
#define CP_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../dataStructures.h"
#include "../globals.h"
#include "../inodeUtil.h"
#include "../blockUtil.h"
#include "write.h"
#include "open.h"


int mycp(Command* command);

#endif