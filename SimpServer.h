/*
 * Charlotte Fedderly
 * V00729142
 * CSC 361 Summer 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

#include "util.h"

#define BUFFER_SIZE 255

/* Request struct */
struct http_request {
    char* request;
    char* method;
    char* identifier;
};

enum status_code {
    OK,
    NOT_FOUND,
    NOT_IMPLEMENTED
};

/* Function prototypes */
int init_socket( const char* );
struct http_request* parse_request( const char* );
char* build_http_response( const char*, const char* );
void free_http_request( struct http_request* );

/* TEST CODE */
void print_http_request( struct http_request* req );
