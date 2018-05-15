#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

// Representation for a 2D point with integer coordinates.
typedef struct {
  // Coordinates of the point.
  int x, y;
} Point;

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// List of all points from the input
Point *ptList;

// Number of points in the input.
int ptCount = 0;

// Read the list of all points
void readPoints() {
  // Use a resizable array, increasing capacity as we read
  int capacity = 10;
  ptList = (Point *) malloc( capacity * sizeof( Point ) );

  // Read points until we can't read any more.
  int x, y;
  while ( scanf( "%d%d", &x, &y ) == 2 ) {
    // Grow the point list if needed.
    if ( ptCount >= capacity ) {
      capacity *= 2;
      ptList = (Point *) realloc( ptList, capacity * sizeof( Point ) );
    }
    
    // Add this new point to the end of the list.
    ptList[ ptCount ].x = x;
    ptList[ ptCount ].y = y;
    ptCount += 1;
  }
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
    if ( ( largest - smallest ) < diff ){ 
        printf("Largest: %.2f, Smallest: %.2f, pts[%.2f, %.2f, %.2f]\n", largest, smallest, d0, d1, d2);
        return true;
    } else
        return false;
}
int main( int argc, char *argv[] ) {
  // Read in all the points
  readPoints();
  int eqCount = 0;
  double p0, p1, p2;
  // Make a pipe for getting counts back from our workers.
  int pfd[ 2 ];
  if ( pipe( pfd ) != 0 )
    fail( "Can't create pipe\n" ); 
  int m;
  int count;
  //Grab the number of workers from command line arg.
  m = atoi(argv[1]);
  pid_t pid;
  pid_t pids[m];  
  int finalI;
  //This is to determine, whether or not we have an odd number of indicies.
  //This makes a difference depending on the number of workers we have.
  //Now I can make sure things devide relatively evenly among the workers and the indicies.
  if ( ptCount % 2 == 0 && m != 1){
      if ( m % 2 == 0 ){
        finalI = 0;
      } else{
        finalI = 1;
        //ptCount -= 1;
      }
  } else if ( ptCount % 2 != 0 && m != 1 ) {
      if ( m % 2 == 0 ){
        finalI = 1;
        //ptCount -= 1;
      } else {
        finalI = 0;
      }
  }
  //w will represent the worker number that is currently running through the code. 
  for( int w = 0; w < m; w++){
      //This is what forks the process m amount of times, to get m number of children.
      if ( (pid = pids[w] = fork()) < 0 ){
          fail("Fork Failed\n");
      }
      //The child process code.
      if ( pids[w] == 0 ){
          close( pfd[0] );
          int n = 0;
          int base, cap;
          //They will add the index that we subtracted above in order to get equal distribution. 
          //This way, we go all the way to the end, and dont leave anything off.
          if ( finalI == 1 ){
              base = ((ptCount - 1) / m) * w;   
              cap = ((ptCount - 1) / m) * (w+1);
              if ( w == m - 1 ){
                cap++;
              }
          } else {
              base = (ptCount / m) * w;   
              cap = (ptCount / m) * (w+1);
          }          
          /* Ok so the loop bounds are dependent on both, the # of workers, and the list size.
          * Each worker only gets assigned a certain segment of the list to start from and end at.
          * The idea being: w0 would start at 0 and go to n, and w1 would start at n+1 etc.
          *
          *The change in time appears to be relatively linear, at least from what I have seen so far.
          */
          for( int i = base; i < cap; i++){
            for ( int j = i+1; j <= ptCount - 2; j++){
              for ( int k = j+1; k <= ptCount - 1; k++){
                    //distances between the points, then square root that to get actual distance.
                    p0 = (double)distSquared( i, j );
                    p0 = sqrt(p0);
                    p1 = (double)distSquared( j, k );
                    p1 = sqrt(p1);
                    p2 = (double)distSquared( i, k );
                    p2 = sqrt(p2);
                    if ( check( p0, p1, p2 ) ){
                        n++;
                    }
              }
            }
          }          
          //printf("Worker %d, reporting %d\n", getpid(), n);
          //Locking, writing to, then unlocking the pipe.
          lockf( pids[w], F_LOCK, 0 );
          write( pfd[1], &n, sizeof(n) );
          lockf( pids[w], F_ULOCK, 0 );
          close( pfd[ 1 ] );
          exit( 0 );
      }
  }      
      //The parent process, which waits for each worker to finish, 
      //then reads the pipe after each child writes to it.
      
      if ( pid > 0 ) {
            close( pfd[ 1 ] ); 
            for(int q = m; q > 0; q--){
                waitpid( pids[q], NULL, 0 );  
                read( pfd[0], &count, sizeof(count) );
                eqCount += count;
            }
             close( pfd[0] );          
      }  

  // Report the total number of triangles we found.
  printf("Triangles: %d\n", eqCount);
  
  return 0;
}
