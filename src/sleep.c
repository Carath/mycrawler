#ifndef _DEFAULT_SOURCE // for usleep
#define _DEFAULT_SOURCE
#endif

// Related macros:
// _POSIX_C_SOURCE
// _XOPEN_SOURCE
// _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

// May be Unix dependant. Used for system calls:
#include <unistd.h>

#include "sleep.h"


// Nota bene: Do _not_ store the value as 'useconds_t', for it is unsigned!


// Simple version: will sleep for that whole duration.
void simple_sleep(double desired_cooldown)
{
	usleep(desired_cooldown * 1000000.); // gives more control than sleep().
}


// Adaptive version, supports several cooldown duration. Both values passed as arg
// must be in seconds. Returns the resulting sleep time, in seconds as well:
double adaptive_sleep(double desired_cooldown, double time_elapsed)
{
	double time_to_wait = desired_cooldown - time_elapsed;

	time_to_wait = time_to_wait < 0. ? 0. : time_to_wait;

	// printf("Time left to wait: %.6f s\n", time_to_wait);

	usleep(time_to_wait * 1000000.); // usleep() gives more control than sleep().

	return time_to_wait;
}
