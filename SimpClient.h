#ifndef CLIENT
#define CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "util.h"

#define DEFAULT_PORT "80"

struct parsed_URI {
    char* uri;
    char* protocol;
    char* hostname;
    char* port;
    char* identifier;
};

/* Function prototypes */
int init_connection( struct addrinfo**, const char*, const char* );
int open_connection( struct addrinfo* );
void perform_http( int, char* );
struct parsed_URI* parse_URI( const char* );
void free_parsed_URI( struct parsed_URI* );

/* TEST CODE */
void print_parsed_URI( struct parsed_URI* );

#endif
