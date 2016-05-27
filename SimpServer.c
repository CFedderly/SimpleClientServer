/*
 * Charlotte Fedderly
 * V00729142
 * CSC 361 Summer 2016
 */

#include "SimpServer.h"

int main( int argc, char *argv[] ) {

    // int sockfd, newsockfd, clilen;
    // char buffer[256];

    char* port;
    char* directory;
    int port_num;
    int sockfd;
    int new_sockfd;
    
    int status;
    socklen_t cli_len;
    
    struct sockaddr_in cli_addr;

    //, cli_addr;
    //    int n;

    if ( argc != 3 ) {
        fprintf( stderr, "Usage: ./SimpServer <port_number> <server_directory> \n" );
        exit( 1 );
    }

    port = mmalloc( strlen( argv[1] ) + 1 );
    strncpy( port, argv[1], strlen( argv[1] ) );

    // check that the port is an integer
    port_num = atoi( port );
    if ( port_num == 0 ) {
        fprintf( stderr, "Error parsing port number: %s. Exiting.\n", port );
        free( port );
        exit( 1 );
    }

    directory = mmalloc( strlen( argv[2] ) + 1 );
    strncpy( directory, argv[2], strlen( argv[2] ) );

    // initialize and bind socket to port
    sockfd = init_socket( port );

    if ( sockfd < 0 ) {
        fprintf( stderr, "Error initializing socket. Exiting.\n" );
        free( port );
        free( directory );
        exit( 1 );
    }

    // listen for incoming connections 
    status = listen( sockfd, 5 );
    if ( status < 0 ) {
        fprintf( stderr, "Error listening on socket. Exiting.\n" );
        free( port );
        free( directory );
        exit( 1 );
    }
    cli_len = sizeof( cli_addr );

    // accept connection from client
    new_sockfd = accept( sockfd, ( struct sockaddr* ) &cli_addr, &cli_len );
    if ( new_sockfd < 0 ) {
        fprintf( stderr, "Error accepting client connection. Exiting.\n" );
        free( port );
        free( directory );
        exit( 1 );
    }

    free( port );
    free( directory );
    
    /*

    // If connection is established then start communicating //
    bzero( buffer,256 );
    n = read( newsockfd, buffer, 255 );

    if ( n < 0 ) {
        perror( "ERROR reading from socket" );
        exit( 1 );
    }

    printf( "Here is the message: %s\n", buffer );

    // Write a response to the client //
    n = write( newsockfd, "I got your message", 18);

    if ( n < 0 ) {
        perror( "ERROR writing to socket" );
        exit( 1 );
    }
    */
    
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
