#include <stdlib.h>
#include <stdio.h>
#include "hello.h"

int main() {
  // Get the hello-world message, and print it.
  char *msg = getMessage();
  printf( "%s\n", msg );

  // It's dynamically allocated, so free it when we're done.
  free( msg );
  
  return EXIT_SUCCESS;
}
