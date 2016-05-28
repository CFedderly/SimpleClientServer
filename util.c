/*
 * Charlotte Fedderly
 * V00729142
 * CSC 361 Summer 2016
 */

#include "util.h"

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
