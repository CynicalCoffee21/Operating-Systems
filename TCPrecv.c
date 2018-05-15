#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

int main( int argc, char *argv[] ) {

  // Prepare a description of server address criteria.
  struct addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria));
  addrCriteria.ai_family = AF_INET;
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_STREAM;
  addrCriteria.ai_protocol = IPPROTO_TCP;

  // Lookup a list of matching addresses
  struct addrinfo *servAddr;
  if ( getaddrinfo( NULL, PORT_NUMBER, &addrCriteria, &servAddr) )
    fail( "Can't get address info" );

  // Try to just use the first one.
  if ( servAddr == NULL )
    fail( "Can't get address" );

  // Create a TCP socket
  int servSock = socket( servAddr->ai_family, servAddr->ai_socktype,
                         servAddr->ai_protocol);
  if ( servSock < 0 )
    fail( "Can't create socket" );

  // Bind to the local address
  if ( bind(servSock, servAddr->ai_addr, servAddr->ai_addrlen) != 0 )
    fail( "Can't bind socket" );

  // Tell the socket to listen for incoming connections.
  if ( listen( servSock, 5 ) != 0 )
    fail( "Can't listen on socket" );

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);

  // Wait for a client to connect
  struct sockaddr_storage clntAddr; // Client address
  // Set length of client address structure (in-out parameter)
  socklen_t clntAddrLen = sizeof(clntAddr);

  int sock = accept( servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
  if ( sock < 0 )
    fail( "Can't accept on socket" );

  // Buffer to receive into, but we never really use it.
  char buffer[ BLOCK_SIZE ];

  int recvCount = 0;
  while ( recvCount < SEND_TOTAL ) {
    int bytesToRecv = BLOCK_SIZE;
    if ( recvCount + bytesToRecv > SEND_TOTAL )
      bytesToRecv = SEND_TOTAL - recvCount;

    // Try to read all want from the socket.
    int count = read( sock, buffer, bytesToRecv );
    printf( "Received: %d\n", count );

    recvCount += count;
  }

  close( sock );
  close( servSock );

  return 0;
}
