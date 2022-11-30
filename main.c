/**<main.c>::**/
#include <stdlib.h>
#include <stdio.h>
#include <main.h>

FILE           *source;

int
main(int argc, const char *argv[])
{
    source = stdin;
    
    if(argc > 1)
    	source = fopen(argv[1], "r");

    lookahead = gettoken(source);

    program();

    exit(0);

}
