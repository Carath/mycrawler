#ifndef SETTINGS_H
#define SETTINGS_H


#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))


#ifndef PRINT_FILE_LOCATION
#define PRINT_FILE_LOCATION() \
	printf("-> In function '%s', from file '%s', line %d.\n", __func__, __FILE__, __LINE__)
#endif


// Careful! Changing those settings necessitates to issue a 'make clean', followed by a 'make' call!

#define VERBOSE_MODE 1 // from 0 to 2

#define REPEAT_WARNINGS 0

#define QUERY_COOLDOWN 2.0 // In seconds.


// Setting ALWAYS_REALLOC to 1, allows the pagecontent size to grow, or decrease,
// as to exactly match the needed size. Causes less memory usage, but might be slower.
#define ALWAYS_REALLOC 0


// Setting SIZE_GROWTH_FACTOR to 1., disables the exponential size growth of pagecontent
// when ALWAYS_REALLOC = 0, thus enabling to size to only grow to the exact needed size.
// A factor greater than 1., may allow fewer memory reallocation.
#define SIZE_GROWTH_FACTOR 1.5


typedef enum {FAILURE, NO_ISSUE, SUCCESS} ReturnCode;


#endif
