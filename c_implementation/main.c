#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "c_ga.h"

int main(int argc, char * argv[])
{
	chromo pool[NUM_INDIVIDUALS];

	run_ga(pool);

	print_complete(pool);

	return 0;
}
