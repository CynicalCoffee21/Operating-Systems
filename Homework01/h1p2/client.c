/**
* client.c simply is responsible for sending commands to the server.c
* through message queues, set up by said server.
*@author Ryan Buchanan
*
*/
#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}
/*
* The purpose of client.c is merely to relay messages from the user to the 
*   server and then recieve some kind of comfirmation, which it then prints to the screen.
*/
int main( int argc, char *argv[] ) { 

    //Open the message queue's
    mqd_t serverQueue = mq_open( SERVER_QUEUE, O_WRONLY );
    mqd_t clientQueue = mq_open( CLIENT_QUEUE, O_RDONLY );
    if ( serverQueue == -1 )
        fail("Client Queue failed to create.\n");
    
    char buffer[ MESSAGE_LIMIT + 1 ];
    memset( buffer, '\0', strlen(buffer) );
    char bufferR[ MESSAGE_LIMIT + 1 ];
    memset( bufferR, '\0', strlen(bufferR) );

   
    
    for(int i = 1; i < argc; i++){
        strcat(buffer, argv[i]);
        strcat(buffer, " ");
    }
    //Relay the user's message to the server, and hopefully get a response.
    mq_send( serverQueue, buffer, strlen( buffer ), 0 );
    sleep(1); //So the server has plenty of time to respond.
    int len = mq_receive( clientQueue, bufferR, sizeof(bufferR), NULL );
    
    if( len > 0 ){
        for ( int i = 0; i < len; i++ ){
            printf( "%c", bufferR[ i ] ); 
        }
        printf("\n");
    }
    mq_close( serverQueue );
    mq_close( clientQueue );
  return 0;
}