#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdio.h>

#include "dataStructures.h"

#include "commands/cd.h"
#include "commands/ls.h"
#include "commands/pwd.h"
#include "commands/unlink.h"
#include "commands/mkdir.h"
#include "commands/link.h"
#include "commands/open.h"
#include "commands/close.h"
#include "commands/rmdir.h"
#include "commands/pfd.h"
#include "commands/lseek.h"
#include "commands/simlink.h"
#include "commands/write.h"
#include "commands/read.h"
#include "commands/cat.h"
#include "commands/cp.h"
#include "commands/mv.h"
#include "commands/chmod.h"

void runCommands(Pipe *commands);


#endif
