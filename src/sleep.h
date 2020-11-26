#ifndef SLEEP_H
#define SLEEP_H


// Simple version: will sleep for that whole duration.
void simple_sleep(double desired_cooldown);


// Adaptive version, supports several cooldown duration. Both values passed as arg
// must be in seconds. Returns the resulting sleep time, in seconds as well:
double adaptive_sleep(double desired_cooldown, double time_elapsed);


#endif
