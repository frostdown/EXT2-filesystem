#include "../../header/commands/cat.h"


int mycat(Command *command)
{


    Path src;
    char* srcFilepath = command->tokenizedCommand[1];
    parseFilepath(srcFilepath, &src);
    int inumsrc = 0;
    inumsrc = findInodeIndex(&src, type_File);

char mybuf[1024] = {0}, dummy = 0;  // a null char at end of mybuf[ ]
   int n;

int fd = openhelper(inumsrc, 0); //open file for read

while( n = readFile(fd, &mybuf[0], 1024)){
       mybuf[n] = 0;             // as a null terminated string
       printf("%s", &mybuf); //  <=== THIS works but not good
       //spit out chars from mybuf[ ] but handle \n properly;
   } 

   strcpy(command->tokenizedCommand[0], "close");
   printf("\n");
   runCommands(command);
}