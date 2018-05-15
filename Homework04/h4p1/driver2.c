// Sample driver where a musician has to wait because he plays an
// instrument that's a duplicate of one that's already on the stage.

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

/** Code for Margie */
void *margie( void *arg ) {
  char *myInstrument = "accordion";

  sleep( 1 ); // Wait 1 second before playing
  
  startPlaying( myInstrument );
  
  sleep( 5 );   // Play for 5 seconds.
  
  stopPlaying( myInstrument );
  
  return NULL;
}

/** Code for Rosmarie */
void *rosmarie( void *arg ) {
  char *myInstrument = "tuba";

  sleep( 2 ); // Wait 2 seconds before playing
  
  startPlaying( myInstrument );
  
  sleep( 3 );   // Play for 3 seconds.
  
  stopPlaying( myInstrument );
  
  return NULL;
}

/** Code for Chase */
void *chase( void *arg ) {
  char *myInstrument = "tuba";

  sleep( 3 ); // Wait 3 seconds before playing
  
  startPlaying( myInstrument );
  
  sleep( 2 );   // Play for 2 seconds.
  
  stopPlaying( myInstrument );
  
  return NULL;
}

/** Code for Dona */
void *dona( void *arg ) {
  char *myInstrument = "theremin";

  sleep( 4 ); // Wait 4 seconds before playing
  
  startPlaying( myInstrument );
  
  sleep( 4 );   // Play for 4 seconds.
  
  stopPlaying( myInstrument );
  
  return NULL;
}

int main( int argc, char *argv[] ) {
  // Initialize our monitor.
  initCombonator();

  // Make a few threads
  pthread_t thread[ 4 ];
  if ( pthread_create( thread + 0, NULL, margie, NULL ) != 0 ||
       pthread_create( thread + 1, NULL, rosmarie, NULL ) != 0 ||
       pthread_create( thread + 2, NULL, chase, NULL ) != 0 ||
       pthread_create( thread + 3, NULL, dona, NULL ) != 0 )
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