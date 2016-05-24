#ifndef CLIENT
#define CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "util.h"

struct parsed_URI {
    char* protocol;
    char* hostname;
    int port;
    char* identifier;
};

/* Function prototypes */
struct parsed_URI* parse_URI( char* );
void perform_http( int, char* );
int open_connection( char* , int );

#endif
