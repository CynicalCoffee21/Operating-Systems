#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>    // for usleep
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit
#include <semaphore.h> // for posix semaphores
#include <sys/time.h>
#include "combonator.h"

bool slotsOpen[] = {true, true, true};
char **Instruments;
pthread_cond_t wait_for_slot;
pthread_mutex_t lock, mon;
int curr_open_slot;
time_t playTimes[4] = {0, 0, 0, 0};
struct timeval endTime;
struct timeval startTime;
/** Used for comparing the incoming instrument to the ones currently
playing.*/
bool compareInstruments( char const *instrument ){
  //only 3 instuments are possible at one time.
  for( int i = 0; i < 3; i++ )
    if(strncmp(Instruments[i], instrument, strlen(instrument) ) == 0)
      return true;
  return false;
}
/** Checks the instruments list to see which is empty */
int findSlot(){
  for( int i = 0; i < 3; i++ )
    if( slotsOpen[i] )
      return i;
  return -1;
}
/** Prints the current set of instruments and the time they have been playing */
void printInstruments(){
  char *printOut = malloc(sizeof(char) * 500);
  int count = 0;
  strcat(printOut, "Now Playing: ");
  for( int i = 0; i < 3; i++ ){
      if(strlen(Instruments[i]) != 0){
          strcat( printOut, (Instruments[i]) );
          strcat( printOut, " " );
          count++;
      }
  }
  strcat(printOut, "\n");

  //Add the time here. getTimeOfDay();

  printf("%s", printOut);
  gettimeofday(&endTime, NULL);
  time_t end = ((endTime.tv_sec * 1000) + (endTime.tv_usec / 1000));
  time_t start = ((startTime.tv_sec * 1000) + (startTime.tv_usec / 1000));
  time_t timePrint = end - start;
  playTimes[count] += timePrint;
  printf("%ld ms\n", timePrint);
}
/** Called by a thread when it wants to start playing.  If the stage
    is full (already has three musicians) or if there's already
    another thread playing the given instrument, then the calling
    thread will wait in this function until it can take the stage. */
void startPlaying( char const *instrument ){
  pthread_mutex_lock( &mon );
  // If stage is full, wait for an instrument to leave
  //OR it the current instrument is already playing, also wait
  while( (!slotsOpen[0] && !slotsOpen[1] && !slotsOpen[2]) ||
          compareInstruments(instrument))
  pthread_cond_wait( &wait_for_slot, &mon );

  pthread_mutex_lock( &lock );

  curr_open_slot = findSlot();
  //Collect the amount of time the current ensemble has been playing.

  printInstruments(); //Need to print now, so we can add the next instrument.

  //Set instrument at the currently open slot to the incoming instrument
  strncpy( Instruments[curr_open_slot], instrument, strlen(instrument) );
  slotsOpen[curr_open_slot] = false;
  gettimeofday(&startTime, NULL);
  //Need a better way to determin where to add the string than the
  // current int.
  pthread_mutex_unlock( &lock );
  pthread_mutex_unlock( &mon );
  //Everytime a player enters the set successfully, print out the last
  //ensemble and their time. Add them to the list and set the slot to false.
}
/** Called by a thread when it wants to stop playing and leave the stage. */
void stopPlaying( char const *instrument ){
  pthread_mutex_lock( &mon );
  //printf("entering loop\n");
  for(int i = 0; i < 3; i++){
    if(strcmp( Instruments[i], instrument ) == 0){
      printInstruments(); //print
      //printf("Match Found\n");
      pthread_mutex_lock( &lock );
      //Ctricial Section
      //printf("%s\n", Instruments[i]);
      memset( Instruments[i], 0, 100 ); //erase
      gettimeofday(&startTime, NULL);
      //printInstruments(); //print
      //printf("Deletion\n");
      slotsOpen[i] = true; //open slot
      curr_open_slot = i; //set slot #
      //Critical Section
      pthread_mutex_unlock( &lock );
      break;
    }
  }
  //Make the thread stop and leave
  pthread_cond_broadcast( &wait_for_slot );
  pthread_mutex_unlock( &mon );
}
/** Interface for the jazz combo monitor.  If I was programming in
      C++, this would all be wrapped up in a class.  Since we're using
      C, it's just a collection of functions, with an initialization
      function to init the state of the whole monitor and a destroy
      function to free its resources. */

/** Initialize the monitor. */
void initCombonator(){
  pthread_cond_init( &wait_for_slot, NULL);
  pthread_mutex_init( &lock, NULL );
  pthread_mutex_init( &mon, NULL );

  //Initialize the instrtuments ** with something like 1024 idk
  Instruments = malloc(sizeof(char *) * 3);
  //This is probably wrong ^^ vv
  for( int i = 0; i < 3; i++ )
    Instruments[i] = malloc(sizeof(char) * 100);
  curr_open_slot = 0;
  //collect the time at initialization.
  gettimeofday(&startTime, NULL);
}
/** Destroy the monitor, freeing any resources it uses.  When these
    monitor is destroyed, it gvies a summary of how long each type of
    ensemble was playing (either silence, a solo, a duet or a
    trio). */
void destroyCombonator(){
  printInstruments();
  for( int i = 0; i < 3; i++ ){
    free(Instruments[i]);
  }
  printf("Summary \n");
  printf("silence: %ld\n", playTimes[0]);
  printf("   solo: %ld\n", playTimes[1]);
  printf("   duet: %ld\n", playTimes[2]);
  printf("   trio: %ld\n", playTimes[3]);
  //Print out the total time played at each level of the ensemble. 0 1 2 3.

  free(Instruments);
}
