#include <stdio.h>
#include <unistd.h>    // for write
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Define the start routines for your three threads.  Each thread will
// use two calls to write() to print its two characters, then it will
// terminate.

void *threadFxn0( void *arg ){
    write( STDOUT_FILENO, "a", 1 );
    write( STDOUT_FILENO, "b", 1 );
    return NULL;
}
void *threadFxn1( void *arg ){
    write( STDOUT_FILENO, "c", 1 );
    write( STDOUT_FILENO, "d", 1 );
    return NULL;
}
void *threadFxn2( void *arg ){
    write( STDOUT_FILENO, "e", 1 );
    write( STDOUT_FILENO, "f", 1 );
    return NULL;
}
int main( int argc, char *argv[] ) {
  // A bunch of times.
  for ( int i = 0; i < 100000; i++ ) {
    // Make three threads.
    pthread_t threads[3];      
    if ( pthread_create( threads + 0, NULL, threadFxn0, NULL) != 0 )
        fail("cannot create thread0");
    if ( pthread_create( threads + 1, NULL, threadFxn1, NULL) != 0 )
        fail("cannot create thread1");
    if ( pthread_create( threads + 2, NULL, threadFxn2, NULL) != 0 )
        fail("cannot create thread2");    
    // Join with the three threads.
    for ( int j = 0; j < 3; j++ ){
        pthread_join( threads[j], NULL );
    }
    // Use the write system call to print out a newline.  The string
    // we're passing to write is null terminated (since that's what
    // double quotesd strings are in C), but we're just using the
    // first byte (the newline).  Write doesn't care about null
    // terminated strings, it just writes out any sequence of bytes
    // you ask it to.
    write( STDOUT_FILENO, "\n", 1 );
  }
  return 0;
}