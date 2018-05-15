// Multi-threaded program where threads try to go through a traffic circle
// implemented as a monitor.

#include <stdio.h>
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit
#include <unistd.h>    // for sleep/usleep
#include <stdbool.h>   // for bool

#include "combonator.h"

// Used to tell the threads when they can exit.
static bool running = true;

// General purpose fail function.  Print a message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

/** Code for Herb */
void *herb( void *arg ) {
  char *myInstrument = "trombone";
  
  while ( running ) {
    // Take a little break.
    usleep( 31000 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 10000 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

/** Code for Beatrice */
void *beatrice( void *arg ) {
  char *myInstrument = "banjo";
  
  while ( running ) {
    // Take a little break.
    usleep( 22500 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 12000 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

/** Code for Sal */
void *sal( void *arg ) {
  char *myInstrument = "marimba";
  
  while ( running ) {
    // Take a little break.
    usleep( 24500 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 11000 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

/** Code for Berry */
void *berry( void *arg ) {
  char *myInstrument = "cello";
  
  while ( running ) {
    // Take a little break.
    usleep( 23000 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 9500 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

/** Code for Patrice */
void *patrice( void *arg ) {
  char *myInstrument = "serpent";
  
  while ( running ) {
    // Take a little break.
    usleep( 37000 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 5000 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

/** Code for Fernando */
void *fernando( void *arg ) {
  char *myInstrument = "trombone";
  
  while ( running ) {
    // Take a little break.
    usleep( 26000 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 12000 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

/** Code for Olga */
void *olga( void *arg ) {
  char *myInstrument = "banjo";
  
  while ( running ) {
    // Take a little break.
    usleep( 26000 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 8000 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

/** Code for Kishaa */
void *kisha( void *arg ) {
  char *myInstrument = "cowbell";
  
  while ( running ) {
    // Take a little break.
    usleep( 19000 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 10000 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

/** Code for Antony */
void *antony( void *arg ) {
  char *myInstrument = "cello";
  
  while ( running ) {
    // Take a little break.
    usleep( 19000 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 9500 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

/** Code for Katie */
void *katie( void *arg ) {
  char *myInstrument = "flute";
  
  while ( running ) {
    // Take a little break.
    usleep( 21000 );

    // Play for a little while.
    startPlaying( myInstrument );
    
    usleep( 13000 );

    stopPlaying( myInstrument );
  }

  return NULL;
}

int main( int argc, char *argv[] ) {
  // Initialize our monitor.
  initCombonator();

  // Make a few threads
  pthread_t thread[ 10 ];
  if ( pthread_create( thread + 0, NULL, herb, NULL ) != 0 ||
       pthread_create( thread + 1, NULL, beatrice, NULL ) != 0 ||
       pthread_create( thread + 2, NULL, sal, NULL ) != 0 ||
       pthread_create( thread + 3, NULL, berry, NULL ) != 0 ||
       pthread_create( thread + 4, NULL, patrice, NULL ) != 0 ||
       pthread_create( thread + 5, NULL, fernando, NULL ) != 0 ||
       pthread_create( thread + 6, NULL, olga, NULL ) != 0 ||
       pthread_create( thread + 7, NULL, kisha, NULL ) != 0 ||
       pthread_create( thread + 8, NULL, antony, NULL ) != 0 ||
       pthread_create( thread + 9, NULL, katie, NULL ) != 0 )
    fail( "Can't make a thread we need.\n" );

  // Let the threads run for a little while.
  sleep( 10 );
  running = false;

  // Wait until all the threads finish.
  for ( int i = 0; i < sizeof( thread ) / sizeof( thread[ 0 ] ); i++ )
    pthread_join( thread[ i ], NULL );

  // Free any monitor resources.
  destroyCombonator();

  return 0;
}