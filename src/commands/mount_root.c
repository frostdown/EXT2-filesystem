

#include "../../header/commands/mount_root.h"


int mount_root()
{
    // load root node into minode table
    root = iget(dev, 2);
}


