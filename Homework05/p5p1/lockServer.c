#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

/** Port number used by my server */
#define PORT_NUMBER "26044"
pthread_cond_t wait_for_unlock;
pthread_mutex_t wait_for_lockCMD, wait_for_unlockCMD, mon;
char **list;

int size = 0;
// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

/** handle a client connection, close it when we're done. */
void *handleClient( void *sock ) {
  // Here's a nice trick, wrap a C standard IO FILE around the
  // socket, so we can communicate the same way we would read/write
  // a file.
  FILE *fp = fdopen( *(int *)sock, "a+" );

  // Prompt the user for a command.
  fprintf( fp, "cmd> " );

  char cmd[ 35 ];
  int match;
  while ( ( match = fscanf( fp, "%[0-9a-zA-Z ]", cmd ) ) == 1 &&
          strcmp( cmd, "quit" ) != 0 ) {
    /*
    * Handle the lock command.
    */
    if(strncmp(cmd, "lock", strlen("lock")) == 0){
      char name[25];
      strncpy(name, cmd+5, 24);
      for(int i = 0; i < size; i++){
        if(strncmp(list[i], name, strlen(name)) == 0){

          pthread_cond_wait( &wait_for_unlock, &mon );

        }
      }
      pthread_mutex_lock( &wait_for_lockCMD );
      //Add the new name to the list.
      strncpy(list[size], name, strlen(name));
      size++;
      //fprintf( fp, "%s\n", list[size - 1] );

      pthread_mutex_unlock( &wait_for_lockCMD );
    }
    /*
    * Handle the unlock command.
    */
    else if(strncmp(cmd, "unlock", strlen("unlock")) == 0){
      char name[25];
      strncpy(name, cmd+7, 24);
      //Search for the given name
      for(int i = 0; i < size; i++){
        if(strncmp(list[i], name, strlen(name)) == 0){
          pthread_mutex_lock( &wait_for_unlockCMD );
          //Start the removal process
          for(int j = i; j < size; j++){
            strncpy(list[j], list[j+1], 24);
          }
          memset(list[size-1], 0, 25);
          size--;
        }
        pthread_mutex_unlock( &wait_for_unlockCMD );
        pthread_cond_broadcast( &wait_for_unlock );
      }
    }
    /*
    * Handle the list command.
    */
    else if(strncmp(cmd, "list", strlen("list")) == 0){
      for(int i = 0; i < size; i++){
        fprintf(fp, "%s\n", list[i]);
      }
      //fprintf(fp, "Size: %d\n", size);
    }

    // Prompt the user for the next command.
    fprintf( fp, "cmd> " );
    fflush( fp );
  }

  // Close the connection with this client.
  fclose( fp );

  return NULL;
}

int main() {
  list = (char **)malloc(sizeof(char *) * 50);
  for(int i = 0; i < 50; i++){
    list[i] = (char *)malloc(sizeof(char) * 25);
  }
  // Prepare a description of server address criteria.
  struct addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria));
  addrCriteria.ai_family = AF_INET;
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_STREAM;
  addrCriteria.ai_protocol = IPPROTO_TCP;

  pthread_cond_init( &wait_for_unlock, NULL );
  pthread_mutex_init( &wait_for_lockCMD, NULL );
  pthread_mutex_init( &wait_for_unlockCMD, NULL );
  pthread_mutex_init( &mon, NULL );
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

  // Fields for accepting a client connection.
  struct sockaddr_storage clntAddr; // Client address
  socklen_t clntAddrLen = sizeof(clntAddr);

  while ( true  ) {
    // Accept a client connection.
    int sock = accept( servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
    pthread_t t_id;
    if(pthread_create( &t_id, NULL, handleClient, (void*) &sock) < 0){
      perror("thread failed");
      return  1;
    }
    pthread_detach(t_id);
    // Talk to this client
    //handleClient( sock );
  }

  // Stop accepting client connections (never reached).
  close( servSock );
  for(int i = 0; i < 50; i++){
    free(list[i]);
  }
  free(list);
  return 0;
}
