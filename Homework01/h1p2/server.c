/**
* The server maintains the playlist of no more than 10 songs, and 
* receives/responds to the message queue's sent by client.c
*
* I chose to malloc the song names bcause simply adding them directly 
* caused a segfault and using a normal char array caused an overwrite.
*
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
/* Initialize the playlist. 
 * I didn't want these to be globals, but I couldn't find any other
 * way to use them in the sigaction handler.
 */
char *playlist[LIST_MAX];
int size = 0;
//The handler function for the ctrl-c replacement.
void handler( int signum ){
    if(signum == SIGINT){
        printf("\n");
        if( size > 0 ){
            for( int i = 0; i < size; i++ ){
                printf("%s\n", playlist[i]);
                free(playlist[i]);
            }
        }
        exit( 0 );
    }
}
// Flag that's true as long as the server should keep running.
static int running = 1;

int main( int argc, char *argv[] ) {
    
  //The sigaction struct and configuration to replace ctrl-c.
  struct sigaction a;
  a.sa_handler = handler;
  a.sa_flags = 0;
  sigaction(SIGINT, &a, 0);  
  // Remove both queues, in case, last time, this program terminated
  // abnormally with some queued messages still queued.
  mq_unlink( SERVER_QUEUE );
  mq_unlink( CLIENT_QUEUE );

  // Prepare structure indicating maximum queue and message sizes.
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = MESSAGE_LIMIT;
  
  // Make both the server and client message queues.
  mqd_t serverQueue = mq_open( SERVER_QUEUE, O_RDONLY | O_CREAT, 0600, &attr );
  mqd_t clientQueue = mq_open( CLIENT_QUEUE, O_WRONLY | O_CREAT, 0600, &attr );
  if ( serverQueue == -1 || clientQueue == -1 )
    fail( "Can't create the needed message queues" );

  //The buffer for messages, and the success status of the commands.
  char buffer[ MESSAGE_LIMIT ];  
  bool success = false;
  
  // Repeatedly read and process client messages.
  for(int j = 0; j < LIST_MAX; j++){
      playlist[j] = malloc(sizeof(char) * SONG_MAX);
  }
  
  while ( running ) {
    memset( buffer, '\0', strlen(buffer) );   
    int len = mq_receive( serverQueue, buffer, sizeof(buffer), NULL );
    //If the server receives the message from client, then do the following.
    if ( len >= 0 ){
        /*The following jumbled mess of code accomplishes a couple of things.
        * 1.) Determine which command was sent through the message queue.
        *      Only 4 possible options: next, last, forward, and playing.
        * 2.) Act accordingly to the previously determined command.
        *      forward will delete the current 0th position in the list and move
        *      the others up, etc.
        * 3.) If successful, the "success" message will be sent through the MQ
        * 4.) If not, then the respective "error" message will be sent.
        */
        if( strncmp(buffer, "next", strlen("next") ) == 0 ){//The NEXT command
            if ( size >= LIST_MAX ){
                mq_send(clientQueue, "error: list full", strlen("error: list full"), 0);
            } 
            else if (size == 0 || size == 1){
                //Create and add a new song to the list.
                char newSong[ SONG_MAX ];
                strncpy( newSong, buffer+5, SONG_MAX );//5 is the length of "next" + a space
                strncpy( playlist[ size ], newSong, strlen(newSong) );
                size++;
                success = true;
            } 
            else {    
                //This will reorganize the list, and finally add the song to index 1.
                for(int i = size; i >= 1; i--){
                    if ( i == 1 ){
                        char newSong[ SONG_MAX ];
                        strncpy( newSong, buffer+5, SONG_MAX );                        
                        strncpy( playlist[ 1 ], newSong, strlen(newSong) );
                        size++;
                        success = true;
                    } 
                    else {
                        //Index at 3 becomes whats at 2, and 2 becomes 1, etc.
                        strncpy( playlist[ i ], playlist[i-1], strlen(playlist[i-1]) );
                    }
                }
            }
        } 
        else if( strncmp(buffer, "last", strlen("last") ) == 0 ){//The LAST command
            if ( size >= LIST_MAX ){
                mq_send(clientQueue, "error: list full", strlen("error: list full"), 0);
            } 
            else if (size == 0){
                //Create and add a new song to the list.
                char newSong[ SONG_MAX ];
                strncpy( newSong, buffer+5, SONG_MAX );//5 is the length of "next" + a space
                strncpy( playlist[0], buffer+5, SONG_MAX );
                size++;
                success = true;
            } 
            else {                
                //Create and add a new song to the list.
                char newSong[ SONG_MAX ];
                memset( newSong, '\0', strlen(newSong) );
                strncpy( newSong, buffer+5, SONG_MAX );//5 is length of "last" + a space   
                strncpy( playlist[ size ], newSong, strlen(newSong) );                
                size++;
                success = true;
            }
        } 
        else if( strncmp(buffer, "forward", strlen("forward") ) == 0 ){//FORWARD
                if( size == 0 ){
                    mq_send(clientQueue, "error: list empty", strlen("error: list empty"), 0); 
                } else {
                    //Reorganize the playlist, so that the first index is simply overwritten.
                    for(int i = 0; i < size; i++){
                        if ( i == 0 ){
                            memset( playlist[0], '\0', strlen(playlist[0]) );
                        } else
                            strncpy( playlist[ i-1 ], playlist[i], SONG_MAX );
                    }
                    size--;
                    success = true;
                }
        } 
        else if( strncmp(buffer, "playing", strlen("playing") ) == 0 ){//PLAYING
                if( size == 0 ){
                    mq_send(clientQueue, "error: list empty", strlen("error: list empty"), 0); 
                } else {
                    mq_send(clientQueue, playlist[ 0 ], strlen(playlist[ 0 ]), 0); 
                }
        } else {// If the message doesn't match any of the above, then this gets sent.
            mq_send(clientQueue, "invalid message", strlen("invalid message"), 0);
        }   
        //Determine whether the success message should be sent.
        //If yes, send it and reset the variable back to false.
        if( success == true ){
            mq_send(clientQueue, "success", strlen("success"), 0);
            success = false;
        }
    } 
  }

  // Close our two message queues (and delete them).
  mq_close( clientQueue );
  mq_close( serverQueue );
  mq_unlink( SERVER_QUEUE );
  mq_unlink( CLIENT_QUEUE );

  return 0;
}