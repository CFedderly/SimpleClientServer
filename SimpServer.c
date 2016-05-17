/*
 * Charlotte Fedderly
 * V00729142
 * CSC 361 Summer 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"

#define MAX_STR_LEN 120         /* maximum string length */
#define SERVER_PORT_ID 9898     /* server port number */

/* Function prototypes */
void cleanExit();
void perform_http( int );

/*---------------------main() routine--------------------------*
 * tasks for main
 * generate socket and get socket id,
 * max number of connection is 3 (maximum length the queue of pending connections may grow to)
 * Accept request from client and generate new socket
 * Communicate with client and close new socket after done
 */

int main( int argc, char** argv ) {
    int newsockid; /* return value of the accept() call */

    while (1) {
        close(newsockid);
    }
    return 0;
}

/*
 * cleans up opened sockets when killed by a signal.
 */
void cleanExit() {
    exit( 0 );
}

/*
 * Accepts a request from "sockid" and sends a response to "sockid".
 */
void perform_http( int sockid ) {

}
