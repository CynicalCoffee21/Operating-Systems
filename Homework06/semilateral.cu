//Elapsed Real Time for input-4.txt: 0.603, 0.606, 0.599
//Elapsed Real Time for input-5.txt: 1.419, 1.391, 1.408

#include <stdio.h>
#include <stdbool.h>
#include <cuda_runtime.h>

// Representation for a 2D point with integer coordinates.
typedef struct {
  // Coordinates of the point.
  int x, y;
} Point;
// Return the squared distance between point i and point j in the ptList.
__device__ double distSquared(Point *devPtList, int i, int j ) {
  double dx = (double)devPtList[ i ].x - (double)devPtList[ j ].x;
  double dy = (double)devPtList[ i ].y - (double)devPtList[ j ].y;
  return dx * dx + dy * dy;
}
/**
* This function finds the largest of 3 numbers.
*@param p0 number 1
*@param p1 number 2
*@param p2 number 3
*@return the largest of the 3 parameters
*/
__device__ double findLargest( double p0, double p1, double p2){
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
__device__ double findSmallest( double p0, double p1, double p2){
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
__device__ bool check( double d0, double d1, double d2 ){
    double largest = findLargest( d0, d1, d2 );
    double smallest = findSmallest( d0, d1, d2 );
    double diff = (smallest * .10);
  //  printf("Largest: %.2f, Smallest: %.2f, pts[%.2f, %.2f, %.2f]\n", largest, smallest, d0, d1, d2);
    if ( ( largest - smallest ) < diff ){
        return true;
    } else
        return false;
}
__global__ void countTriangles( int *devCountList, Point *devPtList, int n ) {
  //for(int k = 0; k < n; k++)
    // printf("%d, %d\n", devPtList[k].x, devPtList[k].y);
  // Unique index for this worker, it's the index of the first point
  // in any triangles we're supposed to find.
  int a = blockDim.x * blockIdx.x + threadIdx.x;
  int total = 0;
  // Make sure I actually have something to work on.
  if ( a < n ) {
       //If the index is above 2, then we can check the points.
       double p0, p1, p2;
       //The loop starts comparing : arr[a], arr[i], arr[j]
       //So it effectively compares a to all following indicies.
       for( int i = a + 1; i < n - 1; i++ ){
         for( int j = i + 1; j < n; j++ ){
           p0 = (double)distSquared(devPtList, a, i);
           p0 = sqrt(p0);
           p1 = (double)distSquared(devPtList, i, j);
           p1 = sqrt(p1);
           p2 = (double)distSquared(devPtList, a, j);
           p2 = sqrt(p2);
           if ( check( p0, p1, p2 ) )
               total++;
         }
       }
       //printf("a:%d, pts[%.2f, %.2f, %.2f]\n", a, p0, p1, p2);

  }
  devCountList[a] = total;
}


// List of all points from the input
Point *ptList;
int *countList;
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

// General function to report a failure and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

int main( ) {
  readPoints();
  //for(int k = 0; k < ptCount; k++)
    // printf("%d, %d\n", ptList[k].x, ptList[k].y);
  // ...
  countList = (int *)malloc( ptCount * sizeof(int) );
  //
  //
  Point *devPtList = NULL;
  if ( cudaMalloc((void **)&devPtList, ptCount * sizeof(Point) ) != cudaSuccess )
    fail( "Failed to allocate space for lenght list on device" );
  //copy list to device from host
  if ( cudaMemcpy( devPtList, ptList, ptCount * sizeof(Point),
                    cudaMemcpyHostToDevice) != cudaSuccess )
    fail( "Can't copy list to device from host" );
  int *devCountList = NULL;
  if ( cudaMalloc((void **)&devCountList, ptCount * sizeof(int) ) != cudaSuccess )
    fail( "Failed to allocate space for lenght list on device" );
  // Block and grid dimensions.
  int threadsPerBlock = 250;
  // Round up.
  int blocksPerGrid = ( ptCount + threadsPerBlock - 1 ) / threadsPerBlock;
  // Run our kernel on these block/grid dimensions
  countTriangles<<<blocksPerGrid, threadsPerBlock>>>( devCountList, devPtList, ptCount );
  if ( cudaGetLastError() != cudaSuccess )
    fail( "Failure in CUDA kernel execution." );
    // copy list from device to host
    if ( cudaMemcpy( ptList, devPtList, ptCount * sizeof(Point),
                     cudaMemcpyDeviceToHost) != cudaSuccess )
      fail( "Can't copy list from device to host" );
    // copy list from device to host
    if ( cudaMemcpy( countList, devCountList, ptCount * sizeof(int),
                     cudaMemcpyDeviceToHost) != cudaSuccess )
      fail( "Can't copy list from device to host" );
  // Print out a sample of the resulting values.
  int sum = 0;
  for ( int i = 0; i < ptCount; i++ ){
       sum += countList[ i ];
  }
  printf("Triangles: %d\n", sum);
  //
  cudaFree( devCountList );
  cudaFree( devPtList );
  //
  free( ptList );
  free( countList );

  cudaDeviceReset();
}
