/*
 * Charlotte Fedderly
 * V00729142
 * CSC 361 Summer 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"

#define MAX_STR_LEN 4096
#define MAX_RES_LEN 4096

/* Function prototypes */
void parse_URI( char*, char*, int*, char* );
void perform_http( int, char* );
int open_connection( char* , int );

/* --------- Main() routine ------------
 * three main task will be excuted:
 * accept the input URI and parse it into fragments for further operation
 * open socket connection with specified sockid ID
 * use the socket id to connect sopecified server
 * don't forget to handle errors
 */

int main( int argc, char** argv )
{
    char uri[ MAX_STR_LEN ];
    char hostname[ MAX_STR_LEN ];
    char identifier[ MAX_STR_LEN ];
    int sockid, port;

    parse_URI( uri, hostname, &port, identifier );
    sockid = open_connection( hostname, port );
    perform_http( sockid, identifier );

    return 0;
}

/* Parse URI into "hostname" and resource "identifier" */

void parse_URI( char *uri, char *hostname, int *port, char *identifier ) {

}

/*
 * connect to a HTTP server using hostname and port,
 * and get the resource specified by identifier
 */
void perform_http( int sockid, char *identifier ) {
    /* connect to server and retrieve response */

    close(sockid);
}

/*
 * open_conn() routine. It connects to a remote server on a specified port.
 */

int open_connection( char *hostname, int port ) {

    int sockfd;
    /* generate socket
     * connect socket to the host address
     */
    return sockfd;
}
