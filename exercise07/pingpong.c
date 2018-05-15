#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// Number of iterations for each thread
#define ITERATIONS 500

// Who gets to go next.
int nextTurn = 0;

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Start routines for the two threads.

void *pingThread( void *arg ){
    for ( int i = 0; i < ITERATIONS; i++ ) {
        while ( nextTurn != 0 )
            ;
        nextTurn = 1;
    }
    return NULL;
}
void *pongThread( void *arg ){
    for ( int i = 0; i < ITERATIONS; i++ ) {
        while ( nextTurn != 1 )
            ;
        nextTurn = 0;
    }
    return NULL;
}

int main( int argc, char *argv[] ) {

  // Create each of the two threads.
  pthread_t ping, pong;
  if( pthread_create( &ping, NULL, pingThread, NULL) != 0 )
      fail("Oh dear god not again.");
  if( pthread_create( &pong, NULL, pongThread, NULL) != 0 )
      fail("Oh dear god not again.");
  // Wait for them both to finish.

  pthread_join( ping, NULL );
  pthread_join( pong, NULL );

  return 0;
}