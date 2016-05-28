/*
 * Charlotte Fedderly
 * V00729142
 * CSC 361 Summer 2016
 */

#include "SimpServer.h"

int main( int argc, char *argv[] ) {

    char request[ BUFFER_SIZE ];

    char* port;
    char* directory;
    char* response;
    int port_num;
    int sockfd;
    int new_sockfd;
    
    int status;
    socklen_t cli_len;
    
    struct sockaddr_in cli_addr;
    struct http_request* req;

    // check for correct arguments
    if ( argc != 3 ) {
        fprintf( stderr, "Usage: ./SimpServer <port_number> <server_directory> \n" );
        exit( 1 );
    }

    port = mmalloc( strlen( argv[1] ) + 1 );
    strncpy( port, argv[1], strlen( argv[1] ) + 1 );

    // check that the port is an integer
    port_num = atoi( port );
    if ( port_num == 0 ) {
        fprintf( stderr, "Error parsing port number: %s. Exiting.\n", port );
        free( port );
        exit( 1 );
    }

    directory = mmalloc( strlen( argv[2] ) + 1 );
    strncpy( directory, argv[2], strlen( argv[2] ) + 1 );

    // check that provided directory is valid
    if ( chdir( directory ) < 0 ) {
        fprintf( stderr, "Unable to change to directory: %s. Exiting.\n", directory );
        free( port );
        free( directory );
        exit( 1 );
    }
    
    // initialize and bind socket to port
    sockfd = init_socket( port );
    if ( sockfd < 0 ) {
        fprintf( stderr, "Error initializing socket. Exiting.\n" );
        free( port );
        free( directory );
        exit( 1 );
    }

    // cleanup argument strings
    free( port );
    free( directory );
    
    // listen for incoming connections 
    status = listen( sockfd, 5 );
    if ( status < 0 ) {
        fprintf( stderr, "Error listening on socket. Exiting.\n" );
        exit( 1 );
    }
    
    cli_len = sizeof( cli_addr );

    while ( 1 ) {

        // accept connection from client
        new_sockfd = accept( sockfd, ( struct sockaddr* ) &cli_addr, &cli_len );
        if ( new_sockfd < 0 ) {
            fprintf( stderr, "Error accepting client connection. Exiting.\n" );
            exit( 1 );
        }

        memset( request, 0, sizeof( request ) );

        // get http request from client
        status = read( new_sockfd, request , BUFFER_SIZE - 1 );
        if ( status < 0 ) {
            fprintf( stderr, "Error reading from client socket. Exiting.\n" );
            exit( 1 );
        }

        // parse the received HTTP request
        req = parse_request( request );

        if ( req == NULL ) {
            fprintf( stderr, "Unable to parse request: %s. Exiting.\n", request );
            exit( 1 );            
        }

        // respond to the request
        response = build_http_response( req->method, req->identifier );
        status = write( new_sockfd, response, strlen( response ) );
        if ( status < 0 ) {
            fprintf( stderr, "Error writing to client socket. Exiting.\n" );
            free_http_request( req );
            free( response );
            exit( 1 );
        }
        
        free_http_request( req );
        free( response );
    }
    
    return 0;
}

/* Build the addrinfo struct, initialize and bind to a socket */
int init_socket( const char* port ) {

    struct addrinfo hints;
    struct addrinfo *result;
    int sockfd;

    memset( &hints, 0, sizeof( struct addrinfo ) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ( getaddrinfo( NULL, port, &hints, &result) != 0 ) {
        return -1;
    }

    sockfd = socket( result->ai_family, result->ai_socktype, result->ai_protocol);
    if ( sockfd == -1 ) {
        return sockfd;
    }
    
    // bind it to the port passed into addrinfo struct
    if ( bind( sockfd, result->ai_addr, result->ai_addrlen ) < 0 ) {
        return -1;
    }
    
    return sockfd;
}

/* Parses the HTTP request and stores relevant info in a struct */
struct http_request* parse_request( const char* raw_request ) {
    char* copy_request;
    char* tok;
    char* temp_id;
    char* uri;
    
    struct http_request* req;
     
    // allocate memory for http_request struct
    req = mmalloc( sizeof( struct http_request ) );

    // copy http request to struct
    req->request = mmalloc( strlen( raw_request ) + 1 );
    strncpy( req->request, raw_request, strlen( raw_request ) + 1 );

    // copy request for parsing
    copy_request = mmalloc( strlen( raw_request ) + 1 );
    strncpy( copy_request, raw_request, strlen( raw_request ) + 1 );

    // parse the method
    tok = strtok( copy_request, " " );
    req->method = mmalloc( strlen( tok ) + 1 );
    strncpy( req->method, tok, strlen( tok ) + 1 );

    // parse the identifier
    uri = strtok( NULL, " " );
    temp_id = strchr( uri, '/' );
    temp_id = strchr( temp_id + 1, '/' );
    temp_id = strchr( temp_id + 1, '/' );

    if ( temp_id != NULL ) {
        if ( temp_id[0] == '/' ) {
            memmove( temp_id, temp_id + 1, strlen( temp_id ) );
        }
        req->identifier = mmalloc( strlen( temp_id ) + 1 );
        strncpy( req->identifier, temp_id, strlen( temp_id ) + 1 );
        
    } else {
        return NULL;
    }
     
    free( copy_request );

    return req;
}

/* Returns an HTTP response from the given HTTP method and identifier */
char* build_http_response( const char* method, const char* identifier ) {
    size_t total_len;
    char* response;
    char* body = NULL;
    char date_time[ 128 ];
    char server_name[ 128 ];
    enum status_code status;
    char* status_string;
    size_t body_len;
    
    // String constants 
    const char* ok = "HTTP/1.0 200 OK\r\n";
    const char* not_found = "HTTP/1.0 404 Not Found\r\n";
    const char* not_impl = "HTTP/1.0 501 Not Implemented\r\n";
    const char* date = "Date: ";
    const char* server = "Server: ";
    const char* clrf = "\r\n";
    
    char* method_tmp = strlwr( method );
    
    // if the method isn't get, status should be not implemented
    if ( strcmp( "get", method_tmp ) != 0 ) {
        status = NOT_IMPLEMENTED;
    } else {

        FILE* file = fopen( identifier, "r" );

        // If we can open the file specified by the identifier,
        // status should be okay
        if ( file ) {
            status = OK;

            // find length of the file
            fseek( file, 0, SEEK_END );
            body_len = ftell( file );
            rewind( file );
            
            // allocate enough memory for response 
            body = mmalloc( body_len + 1 );

            // read the contents of the file, store as the response body
            if ( fread( body, sizeof( char ), body_len, file ) != body_len ) {
                fprintf( stderr, "Error reading file specified by identifer: %s\n.", identifier );
                free( body );
                return NULL;
            }

            body[ body_len ] = '\0';

            fclose( file );
            
        } else {
            // if the file can't be opened, status is not found
            body_len = 0;
            status = NOT_FOUND;
        }
    }

    // set the correct status string
    switch( status ) {
    case OK:
        status_string = mmalloc( strlen( ok ) + 1 );
        strncpy( status_string, ok, strlen( ok ) + 1 );
        break;
    case NOT_IMPLEMENTED:
        status_string = mmalloc( strlen( not_impl ) + 1 );
        strncpy( status_string, not_impl, strlen( not_impl ) + 1 );
        break;
    case NOT_FOUND:
        status_string = mmalloc( strlen( not_found ) + 1 );
        strncpy( status_string, not_found, strlen( not_found ) + 1 );
        break;
    default:
        status_string = mmalloc( strlen( not_found ) + 1 );
        strncpy( status_string, not_found, strlen( not_found ) + 1 );
        break;
    }

    // find the current GMT time
    time_t t = time( NULL );
    snprintf( date_time, sizeof( date_time ), "%s", asctime( gmtime( &t ) ) ); 

    // get the server name 
    gethostname( server_name, sizeof( server_name ) );
    
    total_len = strlen( status_string ) +
        strlen( server ) + strlen( server_name ) + strlen( clrf ) +
        strlen( date ) + strlen( date_time ) + strlen( clrf ) +
        strlen( clrf ) + body_len;

    // build the response
    response = mmalloc( total_len + 1 );
    response[ 0 ] = '\0';
    strcat( response, status_string );
    strcat( response, server );
    strcat( response, server_name );
    strcat( response, clrf );
    strcat( response, date );
    strcat( response, date_time );
    strcat( response, clrf );
    strcat( response, clrf );
    if ( body != NULL ) {
        strcat( response, body );
    }
    response[ total_len ] = '\0';

    if ( status_string != NULL ) {
        free( status_string );
    }
    if ( body != NULL ) {
        free( body );
    }
    
    return response;
}

/* Frees the memory allocated to the http_request struct */
void free_http_request( struct http_request* req ) {
    if ( req != NULL ) {
        if ( req->request != NULL ) {
            free( req->request );
        }
        if ( req->method != NULL ) {
            free( req->method );
        }
        if ( req->identifier != NULL ) {
            free( req->identifier );
        }
        free( req );
    }
}

/* TEST CODE */
void print_http_request( struct http_request* req ) {
    printf( "request: %s\n", req->request );
    printf( "method: %s\n", req->method );
    printf( "identifier: %s\n", req->identifier );
}
