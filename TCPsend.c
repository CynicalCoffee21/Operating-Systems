#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

/** Simple program to send a bunch of data to a TCP server. */
int main( int argc, char *argv[] ) {
  // Make sure we have an argument.
  if ( argc != 2 )
    fail( "usage: TCPsend <server-host>" );

  // Tell the system what kinds of address we want
  struct addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria));
  addrCriteria.ai_family = AF_INET;
  addrCriteria.ai_socktype = SOCK_STREAM;
  addrCriteria.ai_protocol = IPPROTO_TCP;

  // Lookup a list of matching addresses
  struct addrinfo *servAddr;
  if ( getaddrinfo( argv[ 1 ], PORT_NUMBER, &addrCriteria, &servAddr) != 0 )
    fail( "Can't get address info" );

  // Try to just use the first one.
  if ( servAddr == NULL )
    fail( "Can't get address" );

  // Make a socket, and connect it to the address.
  int sock = socket( servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
  if ( sock < 0 )
    fail( "Can't create socket" );

  // Establish the connection to the echo server
  if ( connect( sock, servAddr->ai_addr, servAddr->ai_addrlen ) != 0 )
    fail( "Can't connect to server" );

  // We're done wiht the address info now.
  freeaddrinfo(servAddr);

  // Buffer to send from, don't really care what's in it.
  char buffer[ BLOCK_SIZE ];

  int sendCount = 0;
  while ( sendCount < SEND_TOTAL ) {
    int bytesToSend = BLOCK_SIZE;
    if ( sendCount + bytesToSend > SEND_TOTAL )
      bytesToSend = SEND_TOTAL - sendCount;

    // Try to write to the socket.
    int count = write( sock, buffer, bytesToSend );
    printf( "Sent: %d\n", count );

    sendCount += count;
  }

  // Close the socket connection
  close( sock );

  return 0;
}
