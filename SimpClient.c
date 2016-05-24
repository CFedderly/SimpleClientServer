/*
 * Charlotte Fedderly
 * V00729142
 * CSC 361 Summer 2016
 */

#include "SimpClient.h"

/* --------- Main() routine ------------
 * three main task will be excuted:
 * accept the input URI and parse it into fragments for further operation
 * open socket connection with specified sockid ID
 * use the socket id to connect sopecified server
 * don't forget to handle errors
 */

int main( int argc, char** argv ) {

    struct parsed_URI* parsed_uri; 
    int sockid;
    
    // Make sure URI is supplied
    if( argc != 2 ) {
        printf( "Usage: ./SimpClient <Uniform Resource Identifier> \n" );
        exit( 1 );
    }
    
    char* raw_uri = argv[1];
    printf( "%s\n", raw_uri );

    parsed_uri = parse_URI( raw_uri );
    sockid = open_connection( parsed_uri->hostname, parsed_uri->port );
    perform_http( sockid, parsed_uri->identifier );

    // Clean up
    free( parsed_uri );
    
    return 0;
}

/* Parse URI into "hostname" and resource "identifier" 
URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
protocol://host[:port]/filepath.
delimiters = ":" / "/" / "?" / "#" / "[" / "]" / "@"
*/

struct parsed_URI* parse_URI( char* raw_uri ) {
    char* tok;
    struct parsed_URI* uri;
    char delim;
    
    uri = mmalloc( sizeof( struct parsed_URI ) );

    return uri;
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
