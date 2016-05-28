Charlotte Fedderly
V00729142
CSC 361 Summer 2016
Assignment 1


FILES
=====

Makefile 
SimpClient.c
SimpClient.h
SimpServer.c
SimpServer.h
util.c - includes utility functions used by both SimpClient and SimpServer
util.h 


BUILD
=====

The makefile contains the following build targets:

make all - builds both SimpServer and SimpClient
make clean - cleans all executable and .o files
make SimpServer - builds only SimpServer
make SimpClient - builds only SimpClient

USAGE
=====

SimpClient
----------

Usage:

./SimpClient <URI>

where <URI> is assumed to be of the form <protocol>://<hostname>[:<portnum>]/<identifier>.
- If no identifier is specified, a single slash "/" is used as an identifier.
- If no port number is specified, number 80 is used.
- If a protocol other than HTTP is used, the client will give an error and exit.
- The client only sends out GET requests
- The client assumes that HTTP responses it receives are well-formed ( contain a set of two CLRF sequences to separate header and body)

The following are some example accepted inputs for the client:

./SimpClient http://www.google.ca:80
./SimpClient http://www.google.ca/
./SimpClient http://www.google.ca
./SimpClient http://www.google.ca/index.html
./SimpClient http://www.google.ca:80/index.html

The hostname could also be replaced with a valid IP address of the form x.x.x.x.

SimpServer
----------

Usage:

./SimpServer <port number> <directory>

- The user must specify a port number, there is no default option
- The server will listen for requests until the user exits the server ( ctrl-c )
- The server assumes that the HTTP requests are well-formed.
- The server will return a "501 Not Implemented" status code if an HTTP method other than GET is requested.

Example usage:

./SimpServer 9898 ~/Documents/Directory/


LOCAL SERVER AND CLIENT EXAMPLE
===============================

If the server and the client are both being run on the same local machine containing the html file ~/testDir/test.html, the following example will yield a "200 OK" status code.

charlotte@charlotte-comp:~/Documents/csc361/p1$ ./SimpServer 9898 ~/testDir &
[1] 7661
charlotte@charlotte-comp:~/Documents/csc361/p1$ ./SimpClient http://127.0.0.1:9898/test.html

---Request begin---
GET http://127.0.0.1:9898/test.html HTTP/1.0
Host: 127.0.0.1
Connection: Keep-Alive

---Request end---
HTTP Request sent, awaiting response...

---Response header---
HTTP/1.0 200 OK
Server: charlotte-comp
Date: Sat May 28 06:30:23 2016

---Response body---
Hello world!


CREDIT
======

Used sample code provided on the CSC361 connex page.

