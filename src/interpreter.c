#include "../header/interpreter.h"

char* commandNames[] = {"ls", "cd", "pwd", "unlink", "link", "mkdir", "rmdir", "touch", "open", "close", "pfd", "lseek", "symlink", "write", "read", "cat", "cp", "mv", "chmod", 0};

int (*commandPointers[])(Command*) = {(int (*)()) ls, cd, pwd, unlink, link, mkDir, rmdir, creat_file, myOpen, myClose, pfd, myLseek, simlink, write_file, myRead, mycat, mycp , mymv, chmod};

// returns the index in the commandPointers array of the given command
int findCommand(char * command)
{
	int i = 0;
	while(commandNames[i])
	{
		if (strcmp(command, commandNames[i]) == 0)
			return i;
		i++;
	}
	return -1;
}


// run the commands given in the pipe.
// WARNING: we are assuming there are no pipes in our input. Pipes are not a requirement of this lab.
void runCommands(Pipe* commands)
{
    int commandIndex = findCommand(commands->command.tokenizedCommand[0]);

    if (commandIndex == -1)
    {
        printf("ERROR: could not find command!\n");
        return;
    }

    int returnValue = commandPointers[commandIndex](&(commands->command));
}

