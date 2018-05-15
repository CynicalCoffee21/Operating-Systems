#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>
// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Representation for a 2D point with integer coordinates.
typedef struct {
  // Coordinates of the point.
  int x, y;
} Point;

// Maximum number of points we can store on ptList.  For this program,
// the list is allocated at a fixed size at the start of execution.
// This letsd so workers start working on Points as soon as we read
// them in, without having to worry about the list moving in memory
// (if, say, it had to get resized).
#define POINT_LIMIT 10000

// List of all points from the input
Point *ptList;

// Number of points total in the buffer..
int ptCount = 0, num = 0, total = 0, workers = 0;

/**
* lock : simple mutex lock so that threads and I/O functions don't access the
*         buffer at the same time. Hopefully, preventing race conditions.
*/
sem_t lock, avail, add;

// Read the lsit of points. Acts as the producer for the program.
void readPoints() {
  // Read points until we can't read any more.
  int x, y;
  while ( scanf( "%d%d", &x, &y ) == 2 ) {
    if ( ptCount >= POINT_LIMIT )
      fail( "Too many points." );
    // Append the point we just read to the list.
    sem_wait( &lock );

    ptList[ ptCount ].x = x;
    ptList[ ptCount ].y = y;
    ptCount++;
    sem_post( &lock );
    sem_post( &avail );
    
  }
  //printf("total points read = %d\n", ptCount);
  //Sentinal Values to let the consumers know that there is no work left.
  for( int i = 0; i < workers; i++){
    if ( ptCount >= POINT_LIMIT )
      fail( "Too many points." );
    sem_wait( &lock );

    ptList[ ptCount ].x = -1;
    ptList[ ptCount ].y = -1;
    ptCount++;
    sem_post( &lock );
    sem_post( &avail );
    
  }
  //printf("total points read after sentinals = %d\n", ptCount);
}
// Return the squared distance between point i and point j in the ptList.
int distSquared( int i, int j ) {
    int dx = ptList[ i ].x - ptList[ j ].x;
    int dy = ptList[ i ].y - ptList[ j ].y;
    return dx * dx + dy * dy;
}
/**
* This function finds the largest of 3 numbers.
*@param p0 number 1
*@param p1 number 2
*@param p2 number 3
*@return the largest of the 3 parameters
*/
double findLargest( double p0, double p1, double p2){
    if ( p0 >= p1 && p0 >= p2 ){
         return p0;
    } else if (p1 >= p2 && p1 >= p0) {
        return p1;
    } else
        return p2;
}
/**
* This function finds the smallest of 3 numbers.
*@param p0 number 1
*@param p1 number 2
*@param p2 number 3
*@return the smallest of the 3 parameters
*/
double findSmallest( double p0, double p1, double p2){
    if ( p0 <= p1 && p0 <= p2 ){
        return p0;
    }else if( p1 <= p2 && p1 <= p0){
        return p1;
    }else
        return p2;
}
/**
* This function determines whether the 3 points are within an acceptable range of each other
* in order to qualify as points of a "semilateral" triangle. That range being +/- 10%.
*@param d0 distance b/w point 1 and point 2
*@param d1 distance b/w point 2 and point 3
*@param d2 distance b/w point 3 and point 1
*@return true/false: true if the difference is acceptable. false if not.
*/
bool check( double d0, double d1, double d2 ){
    double largest = findLargest( d0, d1, d2 );
    double smallest = findSmallest( d0, d1, d2 );
    double diff = (smallest * .10);
  //  printf("Largest: %.2f, Smallest: %.2f, pts[%.2f, %.2f, %.2f]\n", largest, smallest, d0, d1, d2);
    if ( ( largest - smallest ) < diff ){
        return true;
    } else
        return false;
}

/**
* Returns the index of the next available point.
*/
int getWork(){
    sem_wait( &avail );
    return num++;
}
/** Start routine for each worker.
* Acts as the set of consumers for the program, each pulling individually from
* the buffer, and subsequently processing the points pulled.
*/
void *workerRoutine( void *arg ) {
  //bool alreadyThere;
  while( 1 ){
    //Grab the next available point's index.
    int index = getWork();
    //If there are less than 3 total points, don't bother.
    if( index < 2 ){
      continue;
    }
    //If the sentinal values are reached, then we need to exit and die.
    if( ptList[index].x == -1 && ptList[index].y == -1){
      break;
    }
    //If the index is above 2, then we can check the points.
    double p0, p1, p2;
    for( int i = index - 1; i >= 1; i-- ){
      for( int j = i - 1; j >= 0; j-- ){
        //
        p0 = (double)distSquared(index, i);
        p0 = sqrt(p0);
        p1 = (double)distSquared(i, j);
        p1 = sqrt(p1);
        p2 = (double)distSquared(index, j);
        p2 = sqrt(p2);
        if ( check( p0, p1, p2 ) ){
            //printf("Found a Triangle using index: %d\n", index);
            sem_wait( &add );
            total++;
            sem_post( &add );
        }
      }
    }
  }
  return NULL;
}
//Main Function, tells everthing else what to do and when.
//Also creates things, like the threads and semaphores, and also destroys them.
int main( int argc, char *argv[] ) {
  if ( argc != 2 || sscanf( argv[ 1 ], "%d", &workers ) != 1 || workers < 1 )
    fail( "usage: semilateral <workers>" );

  // Allocate our statically-sized list for points.
  ptList = (Point *) malloc( POINT_LIMIT * sizeof( Point ) );
  //Initialize the Semaphores
  sem_init( &lock, 0, 1); //Mutex lock.
  sem_init( &avail, 0, 0); //Available point indicator.
  sem_init( &add, 0, 1); //Mutex lock for adding to the total.
  // Make each of the workers.
  pthread_t worker[ workers ];
  for ( int i = 0; i < workers; i++ ){
    pthread_create(&(worker[i]), NULL, workerRoutine, NULL);
  }
  // Then, start getting work for them to do.
  readPoints();
  // Wait until all the workers finish.
  for ( int i = 0; i < workers; i++ )
    pthread_join(worker[i], NULL);

  // Report the total and release the semaphores.
  printf( "Triangles: %d\n", total );
  free(ptList);
  return EXIT_SUCCESS;
}
