/*
 * Charlotte Fedderly
 * V00729142
 * CSC 361 Summer 2016
 */

#include "util.h"

/* write "size" bytes of "ptr" to "sd" */
int writen( int sd, char *ptr, int size ) {
    int no_left, no_written;

    no_left = size;
    while ( no_left > 0 ) {
        no_written = write( sd, ptr, no_left );
        if ( no_written <=0 ) {
            return(no_written);
        }
        no_left -= no_written;
        ptr += no_written;
    }
    return( size - no_left );
}

/* read "size bytes from "sd" to "ptr" */
int readn( int sd, char *ptr, int size ) {
    int no_left, no_read;
    no_left = size;
    while ( no_left > 0 ) {
        no_read = read( sd, ptr, no_left );
        if ( no_read <0 ) {
            return(no_read);
        }
        if ( no_read == 0 ) {
            break;
        }
        no_left -= no_read;
        ptr += no_read;
    }
    return( size - no_left );
}

/* Custom memory allocation to check for errors. */
void* mmalloc(const size_t size) {

    void* allocated = malloc(size);
    if (allocated == NULL) {
        printf("Error allocating memory. Exiting.\n");
        exit(-1);
    }
    return allocated;
}

/* Function to change string to lowercase */
char* strlwr( const char* str ) {
    int i;
    int len = strlen( str );
    char* temp = mmalloc( len + 1 );
    for ( i = 0; i < len ; i++ ) {
        temp[ i ] = tolower( str[ i ] );
    }
    temp[ len ] = '\0';
    return temp;
}

