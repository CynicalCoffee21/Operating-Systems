#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hello.h"
/*
* Returns a char * containing the message "Hello nrbuchan"
*/
char *getMessage(){
  char *ret = (char *)malloc(sizeof(char) * 25);
  strcat(ret, "Hello nrbuchan.");
  return ret;
}
/*
* Executes the above function, frees the space and exits.
*/
int main(){
  // Get the hello-world message, and print it.
  char *msg = getMessage();

  printf( "%s\n", msg );

  // It's dynamically allocated, so free it when we're done.
  free( msg );

  return EXIT_SUCCESS;
}
