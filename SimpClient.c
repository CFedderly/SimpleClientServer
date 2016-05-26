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
    //    struct sockaddr_in* info;
    char* request;
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

    // get the HTTP request string
    request = build_http_request( parsed_uri->uri, parsed_uri->hostname );

    // send the request, wait for a response
    status = perform_http( sockid, request );
    if ( status == -1 ) {
        fprintf( stderr, "Exiting.\n" );
    }
    
    // Clean up
    free_parsed_URI( parsed_uri );
    free( request );
    
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

int open_connection( const struct addrinfo* info ) {

    int sockfd;
    int status;

    sockfd = socket( info->ai_family, info->ai_socktype, info->ai_protocol );
    if ( sockfd == -1 ) {
        return sockfd;
    }

    status = connect( sockfd, info->ai_addr, info->ai_addrlen );
    if ( status != 0 ) {
        return -1;
    }
    return sockfd;
}

char* build_http_request( const char* uri, const char* hostname ) {

    char* request;
    size_t total_len;

    // string constants
    const char* get = "GET ";
    const char* version = " HTTP/1.0\r\n";
    const char* host = "Host: ";
    const char* host_end = "\r\n";
    const char* connection = "Connection: Keep-Alive\r\n\r\n";

    total_len = strlen( get ) + strlen( uri ) + strlen( version )
        + strlen( host ) + strlen( hostname ) + strlen( host_end )
        + strlen( connection );
    request = mmalloc( sizeof( char ) * ( total_len + 1 ) );

    strcat( request, get );
    strcat( request, uri );
    strcat( request, version );
    strcat( request, host );
    strcat( request, hostname );
    strcat( request, host_end );
    strcat( request, connection );
    request[ total_len ] = '\0';
    
    return request;
}

/*
 * connect to a HTTP server using hostname and port,
 */
int perform_http( int sockid, const char* request ) {

    int bytes;
    char* response;
    
    bytes = send( sockid, request, strlen( request ), 0 );

    print_request( request );

    if ( bytes == -1 ) {
        fprintf( stderr, "Error sending request." );
        return bytes;
    }

    response = mmalloc( sizeof( char ) * BUFFER_SIZE );

    bytes = recv( sockid, response, BUFFER_SIZE - 1, 0 );
    if ( bytes == -1 ) {
        fprintf( stderr, "Error receiving response." );
        free( response );
        return bytes;
    }

    response[ bytes ] = '\0';
    
    print_response( response );

    free( response );
    close( sockid );

    return bytes; 
}

void print_request( const char* request ) {
    const char* start = "\n---Request begin---\n";
    const char* end = "---Request end---\n";
    const char* sent = "HTTP Request sent, awaiting response...\n";

    printf( "%s%s%s%s", start, request, end, sent );
}

void print_response( const char* response ) {
    const char* header_start = "\n---Response header---\n";
    const char* body_start = "---Response body---\n";

    char* copy_response;
    char* header_end;
    char* header;
    char* body;
    
    int header_len;
    int body_len;
    int delim_len;
    
    copy_response = mmalloc( strlen( response ) + 1 );
    strncpy( copy_response, response, strlen( response ) );

    // split response by the header and body, which are separated by "\r\n\r\n"
    char* delims = "\r\n\r\n";
    header_end = strstr( copy_response, delims );

    delim_len = strlen( delims );
    header_len = header_end - copy_response;
    body_len = strlen( response ) - header_len - delim_len;

    header = mmalloc( header_len + 1 );
    strncpy( header, copy_response, header_len );
    header[ header_len ] = '\0';
    
    body = mmalloc( body_len + 1 );
    strncpy( body, copy_response + header_len + delim_len, body_len );
    body[ body_len ] = '\0';

    printf( "%s%s%s%s%s \n", header_start, header, delims, body_start, body );

    free( copy_response );
    free( header );
    free( body );
}

/* Parses URI input provided by the user 
 * Assumes input is of the form protocol://host[:port]/filepath.
 * Returns a parsed_URI struct containing the parsed information
 */
struct parsed_URI* parse_URI( const char* raw_uri ) {

    const char* default_port = "80";
    const char* default_id = "/";
    
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

        // get rid of the leading colon
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
        port = mmalloc( strlen( default_port ) + 1 );
        strncpy( port, default_port, strlen( default_port ) );
        uri->port = port;
    }

    // crop hostname if there is a port number present
    end_index = strlen( hostname ) + 1 - port_len;
    hostname[ end_index ] = '\0';

    uri->hostname = hostname;

    // parse the identifier (everything that's left)
    tok = strtok( NULL, "" );
    // if there isn't one, set the identifer to a single slash
    if ( tok == NULL ) {
        // add a trailing slash to uri if it isn't present
        if ( uri->uri[ strlen( raw_uri ) - 1 ] != '/' ) { 
            uri->uri = realloc( uri->uri, strlen( raw_uri ) + 2 );
            strcat( uri->uri, default_id );
        }
        identifier = mmalloc( strlen( default_id ) + 1 );
        strncpy( identifier, default_id, strlen( default_id ) );
        uri->identifier = identifier;
    } else {
        identifier = mmalloc( strlen( tok ) + 1 );
        strncpy( identifier, tok, strlen( tok ) );
        uri->identifier = identifier;
    }
    // Cleanup
    free( copy_uri );
    
    return uri;
}

void free_parsed_URI( struct parsed_URI* uri ) {
    if ( uri != NULL ) {
        
        if ( uri->uri != NULL ) {
            free( uri->uri );
        }
        if ( uri->protocol != NULL ) {
            free( uri->protocol );
        }
        if ( uri->hostname != NULL ) {
            free( uri->hostname );
        }
        if ( uri->port != NULL ) {
            free( uri->port );
        }
        if ( uri->identifier != NULL ) {
            free( uri->identifier );
        }
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
