// Sample driver that's simple enough that we can tell what should happen.
// The stage fills up after three seconds, and and a new musician has
// to wait for a little while.

#include <stdio.h>
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit
#include <unistd.h>    // for sleep/usleep

#include "combonator.h"

// General-purpose fail function.  Print a message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

/** Code for Shirlene */
void *shirlene( void *arg ) {
  char *myInstrument = "bass";

  sleep( 1 ); // Wait 1 second before playing
  
  startPlaying( myInstrument );
  
  sleep( 4 );   // Play for 4 seconds.
  
  stopPlaying( myInstrument );
  
  return NULL;
}

/** Code for Homer */
void *homer( void *arg ) {
  char *myInstrument = "drums";

  sleep( 2 ); // Wait 2 seconds before playing
  
  startPlaying( myInstrument );
  
  sleep( 4 );   // Play for 4 seconds.
  
  stopPlaying( myInstrument );
  
  return NULL;
}

/** Code for Tamara */
void *tamara( void *arg ) {
  char *myInstrument = "piano";

  sleep( 3 ); // Wait 3 seconds before playing
  
  startPlaying( myInstrument );
  
  sleep( 4 );   // Play for 4 seconds.
  
  stopPlaying( myInstrument );
  
  return NULL;
}

/** Code for Lester */
void *lester( void *arg ) {
  char *myInstrument = "trumpet";

  sleep( 4 ); // Wait 4 seconds before playing
  
  startPlaying( myInstrument );
  
  sleep( 3 );   // Play for 3 seconds.
  
  stopPlaying( myInstrument );
  
  return NULL;
}

int main( int argc, char *argv[] ) {
  // Initialize our monitor.
  initCombonator();

  // Make a few threads
  pthread_t thread[ 4 ];
  if ( pthread_create( thread + 0, NULL, shirlene, NULL ) != 0 ||
       pthread_create( thread + 1, NULL, homer, NULL ) != 0 ||
       pthread_create( thread + 2, NULL, tamara, NULL ) != 0 ||
       pthread_create( thread + 3, NULL, lester, NULL ) != 0 )
    fail( "Can't make a thread we need.\n" );

  // Wait until all the threads finish.
  for ( int i = 0; i < sizeof( thread ) / sizeof( thread[ 0 ] ); i++ )
    pthread_join( thread[ i ], NULL );

  // Let there be one second of silence before shutting down
  // the monitor.
  sleep( 1 );

  // Free any monitor resources.
  destroyCombonator();

  return 0;
}