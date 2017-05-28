#include "../../header/commands/mv.h"

int mymv(Command* command)
{
    link(command);

    unlink(command);
    
}
