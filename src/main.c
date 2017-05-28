
#include <stdio.h>

#include "../header/dataStructures.h"
#include "../header/globals.h"
#include "../header/parser.h"
#include "../header/interpreter.h"
#include "../header/blockUtil.h"

// command includes
#include "../header/commands/mount_root.h"

int init(void)
{
    // initialize all Minodes
    int i = 0;
    for (i = 0; i < NMINODE; i++)
    {
        minode[i].dev = 0;
        minode[i].ino = 0;
        minode[i].refCount = 0;
        minode[i].dirty = 0;
        minode[i].mounted = 0;
        minode[i].mptr = NULL;
    }

    // initialize procs
    proc[0].pid = 1;
    proc[0].uid = 0;
    proc[0].cwd = 0;

    proc[1].pid = 2;
    proc[1].uid = 1;
    proc[1].cwd = 0;

    // set running
    running = &proc[0];

    for (int i = 0; i < NFD; i++)
    {
        proc[0].fd[i] = NULL;
        proc[1].fd[i] = NULL;
    }
}

int main(int argc, char **argv, char **env)
{
    // initialize the filesystem
    if (argc < 2)
    {
        printf("default filesystem\n");
        loadFilesystem("mydisk");
    }
    else
    {
        printf("filesystem:%s\n", argv[1]);
        loadFilesystem(argv[1]);
    }
    loadBlocks();
    checkSuper();

    // init
    init();

    // mount root
    mount_root();

    // set up the running process
    running = &proc[0];
    running->cwd = root;

    while (1)
    {
        // TODO print out filepath based command prompt.
        // get command input
        printf(ANSI_COLOR_CYAN "Command Line $ " ANSI_COLOR_RESET);
        char input[128];
        char cbuf[128];
        memset(input, 0, 128);
        fgets(input, 128, stdin);
        input[strlen(input) - 1] = '\0';
        strcpy(cbuf, input);
        kpathnamehelper(cbuf);

        // parse input
        Pipe *commands;
        parseCommands(input, &commands);

        // check for quit command
        //if (strcmp(commands->command.tokenizedCommand[0], "quit") == 0) //for some reason this is scrambling my char buffers!?!?
        //    return 0;
        // if (strcmp(commands->command.tokenizedCommand[0], "q") == 0)
        //    return 0;

        // run commands
        runCommands(commands);

        // free commands before next input
        freePipe(commands);
    }

    printf("\n"); // so when we exit our shell the actual shell comes up in the right place.

    return 0;
}
