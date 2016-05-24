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
    if ( parsed_uri == NULL ) {
        printf( "Unable to parse uri: %s. Exiting.\n", raw_uri );
        exit( 1 );
    }
    sockid = open_connection( parsed_uri->hostname, parsed_uri->port );
    perform_http( sockid, parsed_uri->identifier );

    // Clean up
    free_parsed_URI( parsed_uri );
    
    return 0;
}

/* Parse URI into "hostname" and resource "identifier" 
URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
protocol://host[:port]/filepath.
delimiters = ":" / "/" / "?" / "#" / "[" / "]" / "@"
*/

struct parsed_URI* parse_URI( const char* raw_uri ) {
    char* copy_uri;
    char* tok;
    char* port;
    char* hostname;
    size_t port_len;
    int port_num;
    int end_index;
    struct parsed_URI* uri;
    
    // allocate memory for parsed_URI struct
    uri = mmalloc( sizeof( struct parsed_URI ) );

    // copy raw URI to store in struct
    uri->uri = mmalloc( strlen( raw_uri ) + 1 );
    strncpy( uri->uri, raw_uri, strlen( raw_uri ) );

    // copy raw URI for parsing
    copy_uri = mmalloc( strlen( raw_uri ) + 1 );
    strncpy( copy_uri, raw_uri, strlen( raw_uri ) );

    // get the protocol
    tok = strtok( copy_uri, ":" );
    printf( "%s \n", tok );

    // parse out the hostname
    tok = strtok( NULL, "/" );
    printf( "%s %zu\n", tok, strlen( tok ) );
    hostname = mmalloc( strlen( tok ) + 1 );
    strncpy( hostname, tok, strlen( tok ) );
    
    // parse out the port, if it's present
    port = strchr( tok, ':' );
    if ( port != NULL ) {
        if ( port[0] == ':' ) {
            memmove( port, port + 1, strlen( port ) );
        }

        printf( "%s %zu\n", port, strlen( port ) );
        port_num = atoi( port );
        port_len = strlen( port ) + 2;
        
        if ( port_num != 0 ) {
            uri->port = port_num;
        } else {
            printf( "Error parsing port num %s. \n", port );
            goto error_cleanup;
        }
        
    } else {
        port_len = 0;
        uri->port = DEFAULT_PORT;
    }

    // crop hostname if needed (port num is present )
    end_index = strlen( hostname) + 1 - port_len;
    hostname[ end_index ] = '\0';
    printf( "hostname: %s\n", hostname );
    uri->hostname = hostname;

    // parse the identifier
    tok = strtok( NULL, "/" );
    printf( "%s \n", tok );
    
    goto cleanup;
    
 // cleanup upon error
 error_cleanup: 
    free_parsed_URI( uri );
    uri = NULL;
 // regular cleanup
 cleanup:
    free( copy_uri );
    
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

void free_parsed_URI( struct parsed_URI* uri ) {
    if ( uri != NULL ) {
        
        if ( uri->uri != NULL ) {
            printf( "freeing uri \n");
            free( uri->uri );
        }
        if ( uri->protocol != NULL ) {
            printf( "freeing proto \n");
            free( uri->protocol );
        }
        if ( uri->hostname != NULL ) {
            printf( "freeing hostname \n");
            free( uri->hostname );
        }
        if ( uri->identifier != NULL ) {
            printf( "freeing id \n");
            free( uri->identifier );
        }
        printf( "freeing uri_struct \n");
        free( uri );
    }
}
