#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

// Number of iterations for each thread
#define ITERATIONS 500

// Declare two anonymous semaphores, one to let each of the threads
// go next.
sem_t pingNext;
sem_t pongNext;

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Start routines for the two threads.
//Run function for the ping thread.
void *pingRun( void *v ){
  for( int i = 0; i < ITERATIONS; i++){
    sem_wait( &pingNext );
    sem_post( &pongNext );
  }
  return NULL;  
}
//Run function for the pong thread.
void *pongRun( void *v ){
  for( int i = 0; i < ITERATIONS; i++){
    sem_wait( &pongNext );
    sem_post( &pingNext );
  }
  return NULL;
}
int main( int argc, char *argv[] ) {

  // Create two semephaores, one to let ping go next and one to let
  // pong go next.
  sem_init(&pingNext, 0, 0);
  sem_init(&pongNext, 0, 1);

  // Create each of the two threads.
  pthread_t ping, pong;
  if ( pthread_create(&ping, NULL, pingRun, NULL) != 0 ||
       pthread_create(&pong, NULL, pongRun, NULL) != 0 )
    fail("Failed to create threads.");

  // Wait for them both to finish.
  pthread_join(ping, NULL);
  pthread_join(pong, NULL);

  // Destroy the two semaphores.
  sem_destroy(&pingNext);
  sem_destroy(&pongNext);

  return 0;
}
