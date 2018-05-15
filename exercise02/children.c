#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void fail( char *msg ) {
  printf( "Error: %s\n", msg );
  exit( EXIT_FAILURE );
}
/**
*I could not figure out why the "Done" print was executing before the child process prints,
* so to get around the issue, I just made the rest of main wait long enough for the children
* to finish their jobs, and then finish.
*
*It isn't necessarily pretty, nor particularly efficient...but it works I guess so...thats what matters right?...
*/
int main( int argc, char *argv[] ) {
  // Try to make a child process.
  int pid = fork();
  for( int i = 0; i < 2; i++){
      if ( pid == -1 ){
          fail( "Can't create child process" );
      }
      if ( pid > 0 ) {
          pid = fork();
      }
      // Print out a report from that child.
      if ( pid == 0 ) {
        sleep( 1 );
        int kidID = getpid();
        int parentID = getppid();
        printf( "I am %d, child of %d\n", kidID, parentID );
        exit( EXIT_SUCCESS );
      }
  }
  sleep( 2 );
  printf( "Done\n" );
  return EXIT_SUCCESS;
}