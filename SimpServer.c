/*
 * Charlotte Fedderly
 * V00729142
 * CSC 361 Summer 2016
 */

#include "SimpServer.h"

int main( int argc, char *argv[] ) {

    // int sockfd, newsockfd, clilen;
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

        printf( "Message: %s\n", request );
        
        req = parse_request( request );

        print_http_request( req );
        
        if ( req == NULL ) {
            fprintf( stderr, "Unable to parse request: %s. Exiting.\n", request );
            exit( 1 );            
        }

        //response = build_http_response( req->method, req->identifier );
        response = "Hello\r\n\r\nHow are you?\n";
        status = write( new_sockfd, response, strlen( response ) );
        if ( status < 0 ) {
            fprintf( stderr, "Error writing to client socket. Exiting.\n" );
            exit( 1 );
        }
        
        free_http_request( req );
        //free( response );
    }
    
    return 0;
}

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

struct http_request* parse_request( const char* raw_request ) {
    char* copy_request;
    char* method;
    char* identifier;
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

    tok = strtok( copy_request, " " );
    method = mmalloc( strlen( tok ) + 1 );
    strncpy( method, tok, strlen( tok ) + 1 );
    
    req->method = method;
         
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

char* build_http_response( const char* method, const char* identifier ) {
    size_t total_len;
    char* response;
    enum status_code status;
    
    // String constants 
    const char* ok = "HTTP/1.0 200 OK\r\n";
    const char* not_found = "HTTP/1.0 404 Not Found\r\n";
    const char* not_impl = "HTTP/1.0 501 Not Implemented\r\n";

    const char* status_string;
    char* body;
    
    char* method_tmp = strlwr( method );
    if ( strcmp( "get", method_tmp ) != 0 ) {
        status = NOT_IMPLEMENTED;
    } else {

        FILE* file = fopen( identifier, "r" );

        if ( file ) {
            status = OK;
        } else {
            status = NOT_FOUND;
        }

    }

    switch( status ) {
    case OK:
        status_string = ok;
        break;
    case NOT_FOUND:
        status_string = not_found;
        break;
    case NOT_IMPLEMENTED:
        status_string = not_impl;
        break;
    default:
        status_string = not_found;
    }

    return "";
}

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

void print_http_request( struct http_request* req ) {
    printf( "request: %s\n", req->request );
    printf( "method: %s\n", req->method );
    printf( "identifier: %s\n", req->identifier );
}
