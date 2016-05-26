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
    struct addrinfo* addr_info;
    struct sockaddr_in* info;
    int sockid;
    int status;
    
    // Make sure URI is supplied
    if( argc != 2 ) {
        fprintf( stderr, "Usage: ./SimpClient <Uniform Resource Identifier> \n" );
        exit( 1 );
    }
    
    parsed_uri = parse_URI( argv[1] );
    if ( parsed_uri == NULL ) {
        fprintf( stderr, "Unable to parse uri: %s. Exiting.\n", argv[1] );
        exit( 1 );
    }

    /* TEST CODE */
    print_parsed_URI( parsed_uri );

    // Protocol must be http
    char* tolower = strlwr( parsed_uri->protocol );
    if ( strcmp( "http", tolower ) != 0 ) {
        fprintf( stderr, "Protocol must be HTTP. Instead, it was %s. Exiting.\n", parsed_uri->protocol );
        free_parsed_URI( parsed_uri );
        exit( 1 );
    }

    // create addrinfo struct
    status = init_connection( &addr_info, parsed_uri->hostname, parsed_uri->port );
    if ( status != 0 ) {
        fprintf( stderr, "Error creating addrinfo struct. Exiting.\n" );
        free_parsed_URI( parsed_uri );
        exit( 1 );
    }

    sockid = open_connection( addr_info );
    if ( sockid == -1 ) {
        fprintf( stderr, "Error creating connecting to socket. Exiting.\n" );
        free_parsed_URI( parsed_uri );
        exit( 1 );        
    }
    perform_http( sockid, parsed_uri->identifier );

    // Clean up
    free_parsed_URI( parsed_uri );
    
    return 0;
}

int init_connection( struct addrinfo** result, const char* hostname, const char* port ) {

    struct addrinfo init;
    int status;
    
    memset( &init, 0, sizeof( struct addrinfo ) );
    init.ai_family = AF_INET;
    init.ai_socktype = SOCK_STREAM;
    status = getaddrinfo( hostname, port, &init, result );

    return status;
}

int open_connection( struct addrinfo* info ) {

    int sockfd;
    int status;

    sockfd = socket( info->ai_family, info->ai_socktype, info->ai_protocol );
    printf( "%d\n", sockfd );
    if ( sockfd == -1 ) {
        return sockfd;
    }

    status = connect( sockfd, info->ai_addr, info->ai_addrlen );
    printf( "%d\n", status );
    if ( status != 0 ) {
        return -1;
    }
    return sockfd;
}

/*
 * connect to a HTTP server using hostname and port,
 * and get the resource specified by identifier
 */
void perform_http( int sockid, char *identifier ) {
    /* connect to server and retrieve response */

    close(sockid);
}

/* Parses URI input provided by the user 
 * Assumes input is of the form protocol://host[:port]/filepath.
 * Returns a parsed_URI struct containing the parsed information
 */
struct parsed_URI* parse_URI( const char* raw_uri ) {
    char* copy_uri;
    char* tok;
    char* protocol;
    char* port;
    char* temp_port;
    char* hostname;
    char* identifier;
    
    size_t port_len;
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
    protocol = mmalloc( strlen( tok ) + 1 );
    strncpy( protocol, tok, strlen( tok ) );
    uri->protocol = protocol;
    
    // parse out the hostname
    tok = strtok( NULL, "/" );
    hostname = mmalloc( strlen( tok ) + 1 );
    strncpy( hostname, tok, strlen( tok ) );
    
    // parse out the port, if it's present
    temp_port = strchr( tok, ':' );
    if ( temp_port != NULL ) {
        if ( temp_port[0] == ':' ) {
            memmove( temp_port, temp_port + 1, strlen( temp_port ) );
        }
        port_len = strlen( temp_port ) + 2;
        port = mmalloc( strlen( temp_port ) + 1 );
        strncpy( port, temp_port, strlen( temp_port ) ); 
        // check that the port is an integer
        if ( atoi( port ) != 0 ) {
            uri->port = port;
        } else {
            printf( "Error parsing port num %s. \n", port );
            free_parsed_URI( uri );
            free( copy_uri );
            return NULL;
        }
        
    } else {
        port_len = 0;
        uri->port = DEFAULT_PORT;
    }

    // crop hostname if there is a port number present
    end_index = strlen( hostname ) + 1 - port_len;
    hostname[ end_index ] = '\0';

    uri->hostname = hostname;

    // parse the identifier (everything that's left)
    tok = strtok( NULL, "" );
    identifier = mmalloc( strlen( tok ) + 1 );
    strncpy( identifier, tok, strlen( tok ) );
    uri->identifier = identifier;
    
    // Cleanup
    free( copy_uri );
    
    return uri;
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

/* TEST CODE */
void print_parsed_URI( struct parsed_URI* uri ) {
    printf( "uri: %s\n", uri->uri );
    printf( "protocol: %s\n", uri->protocol );
    printf( "hostname: %s\n", uri->hostname );
    printf( "port: %s\n", uri->port );
    printf( "identifier: %s\n", uri->identifier );
}
