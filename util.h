#ifndef UTIL
#define UTIL

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

/* Function prototypes */
int writen( int, char*, int );
int readn( int, char*, int );
void* mmalloc( size_t );

#endif

