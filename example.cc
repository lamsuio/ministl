#include <stdio.h>
#include "MiniSTLConfig.h"
#include "ministl/iterator.h"

int main(int argc, char * argv[])
{
    fprintf(stdout, "%s Version %d.%d\n", argv[0], MiniSTL_VERSION_MAJOR, 
            MiniSTL_VERSION_MINOR);
    return (0);
}
