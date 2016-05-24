#ifndef CLIENT
#define CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "util.h"

#define DEFAULT_PORT 80

struct parsed_URI {
    char* uri;
    char* protocol;
    char* hostname;
    int port;
    char* identifier;
};

/* Function prototypes */
struct parsed_URI* parse_URI( const char* );
void perform_http( int, char* );
int open_connection( char* , int );
void free_parsed_URI( struct parsed_URI* );

/* TEST CODE */
void print_parsed_URI( struct parsed_URI* );

#endif
