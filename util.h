#ifndef UTIL
#define UTIL

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/* Function prototypes */
void* mmalloc( size_t );
char* strlwr( const char* );

#endif

