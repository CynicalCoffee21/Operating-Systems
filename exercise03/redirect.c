#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

static void fail( char *msg ) {
  // This program does redirection of standard output.  Good thing
  // we still have standard error around, so we can print error
  // messages even 
  fprintf( stderr, "Error: %s\n", msg );
  exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] ) {
  // Make a child process to run cat.
  pid_t pid = fork();
  if ( pid == -1 )
    fail( "Can't create child process" );

  if ( pid == 0 ) {
    // I'm the child.  Before replacing myself with the cat program,
    // change its environment so it reads from "input.txt" instead of
    // standard input.
    int fileID = open("input.txt", O_RDONLY);
    if( dup2( fileID, STDIN_FILENO ) < 0 )
        fail("Cannot replace file descriptor for stdin\n");
    close(fileID);
    // ... and writes to "output.txt" instead of standard output.
    int fileIDO = open("output.txt", O_RDWR | O_CREAT, S_IRWXU);
    if( dup2( fileIDO, STDOUT_FILENO ) < 0 )
        fail("Cannot replace file descriptor for stdout\n");
    close(fileIDO);
    // Now, run cat.  Even though it thinks it's reading from standard
    // input and writing to standard output, it will really be copying these
    // files.
    execl("/bin/cat", "cat", NULL);
    // ...
  }

  // Wait for the cat program to finish.
  wait( NULL );

  return EXIT_SUCCESS;
}