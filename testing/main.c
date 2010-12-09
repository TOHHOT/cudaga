#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "c_ga.h"

int main(int argc, char * argv[])
{
	chromo pool[NUM_INDIVIDUALS];

	srand(time(NULL));

	run_ga(pool);

	return 0;
}
