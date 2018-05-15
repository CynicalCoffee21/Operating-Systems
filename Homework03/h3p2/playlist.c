#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>

// Maximum number of songs on the playlist.
#define LIST_MAX 10

// Maximum length of the name of a song.
#define SONG_MAX 30


sem_t *mutex;

// Representation for a list of songs.
typedef struct {
  // List of all the titles on the playlist.
  char title[ LIST_MAX ][ SONG_MAX + 1 ];

  // Number of songs currently on the playlist.
  int len;
} Playlist;

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Print out the contents  the whole playlist
void showList( Playlist *list ) {
  #ifndef UNSAFE
    sem_wait( mutex );
  #endif
  for ( int i = 0; i < list->len; i++ )
    printf( "%s\n", list->title[ i ] );
  #ifndef UNSAFE
    sem_post( mutex );
  #endif
}

// Print out just the song that's currently playing.
bool playing( Playlist *list ) {
  #ifndef UNSAFE
    sem_wait( mutex );
  #endif
  if ( list->len == 0 )
    return false;

  printf( "%s\n", list->title[ 0 ] );
  #ifndef UNSAFE
    sem_post( mutex );
  #endif
  return true;
}

// Remove the currently playing song, and slide all the remaining
// ones toward the front of the list.
bool forward( Playlist *list ) {
  #ifndef UNSAFE
    sem_wait( mutex );
  #endif
  if ( ! list->len )
    return false;

  for ( int i = 0; i + 1 < list->len; i++ )
    strcpy( list->title[ i ], list->title[ i + 1 ] );

  list->len -= 1;
  #ifndef UNSAFE
    sem_post( mutex );
  #endif
  return true;
}

// Add a song to be played right after the current one is done, or as the
// current song if the list is empty.
bool addNext( Playlist *list, char *song ) {
  #ifndef UNSAFE
    sem_wait( mutex );
  #endif
  if ( list->len >= LIST_MAX )
    return false;

  // Push everything after the first song up by a slot.
  for ( int i = list->len; i > 1; i-- )
    strcpy( list->title[ i ], list->title[ i - 1 ] );
  list->len += 1;

  if ( list->len == 1 )
    strcpy( list->title[ 0 ], song );
  else
    strcpy( list->title[ 1 ], song );
  #ifndef UNSAFE
    sem_post( mutex );
  #endif
  return true;
}

// Add a song to be played after everything else on the playlist has
// been played.
bool addLast( Playlist *list, char *song ) {
  #ifndef UNSAFE
    sem_wait( mutex );
  #endif
  if ( list->len >= LIST_MAX )
    return false;

  strcpy( list->title[ list->len ], song );
  list->len += 1;
  #ifndef UNSAFE
    sem_post( mutex );
  #endif
  return true;
}

void test( Playlist *list, int n, char *song ) {
  for ( int i = 0; i < n; i++ ) {
    addLast( list, song );
    forward( list );
  }
}

int main( int argc, char *argv[] ) {
  // Get my unique key for my shared memory segment.
  key_t key = ftok( "/afs/unity.ncsu.edu/users/n/nrbuchan", 1 );
  // Get the shared memory segment.  It should already exist.
  int shmid = shmget( key, sizeof( Playlist ), 0600 | IPC_CREAT );

  mutex = sem_open("/nrbuchan-playlist-lock", O_CREAT, 0666, 1);

  if ( mutex == NULL )
    fail( "Cannot create semaphore" );  
  if ( shmid == -1 )
    fail( "Can't open shared memory" );

  // Map it into our memory.
  Playlist *list = (Playlist *)shmat( shmid, 0, 0 );
  if ( list == (void *)-1 )
    fail( "Can't map shared memory segment into address space" );

  // Interpret the command-line arguments.
  if ( argc == 2 && strcmp( argv[ 1 ], "list" ) == 0 ) {
    showList( list );
  } else if ( argc == 2 && strcmp( argv[ 1 ], "playing" ) == 0 ) {
    if ( ! playing( list ) )
      printf( "error: list empty\n" );
  } else if ( argc == 2 && strcmp( argv[ 1 ], "forward" ) == 0 ) {
    if ( ! forward( list ) )
      printf( "error: list empty\n" );
  } else if ( argc == 3 && strcmp( argv[ 1 ], "next" ) == 0 ) {
    if ( ! addNext( list, argv[ 2 ] ) )
      printf( "error: list full\n" );
  } else if ( argc == 3 && strcmp( argv[ 1 ], "last" ) == 0 ) {
    if ( ! addLast( list, argv[ 2 ] ) )
      printf( "error: list full\n" );
  } else if ( argc == 4 && strcmp( argv[ 1 ], "test" ) == 0 ) {
      test( list, atoi(argv[2]), argv[3] );
  } else {
    fprintf( stderr, "usage: playlist list\n" );
    fprintf( stderr, "       playlist playing\n" );
    fprintf( stderr, "       playlist forward\n" );
    fprintf( stderr, "       playlist next <song>\n" );
    fprintf( stderr, "       playlist last <song>\n" );
    exit( 1 );
  }

  // Let go of our shared memory segment.
  shmdt( list );
  sem_unlink( "/nrbuchan-playlist-lock" );
  return 0;
}
