import java.awt.Point;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Scanner;
/**
* The semiThread is a thread object that is  used for counting semiLateral triangles.
*
*/
class semiThread extends Thread{
	private int startIndex;
	private int endIndex;
	private ArrayList<Point> pList;
	private int OGsize;
	public int numFound;
	/**
	 * Use the starting and ending indexes for both traversal and size of the list field.
	 * @param list the main list of points
	 * @param start starting index
	 * @param end ending index
	 */
	public semiThread(ArrayList<Point> list, int start, int end){
		this.startIndex = start;
		this.endIndex = end;
		this.OGsize = list.size();
		this.pList = list;
	}
	/**
	* This function determines whether the 3 points are within an acceptable range of each other
	* in order to qualify as points of a "semilateral" triangle. That range being +/- 10%.
	*@param d0 distance b/w point 1 and point 2 
	*@param d1 distance b/w point 2 and point 3
	*@param d2 distance b/w point 3 and point 1
	*@return true/false: true if the difference is acceptable. false if not.
	*/
	boolean check( double d0, double d1, double d2 ){
	    double largest = findLargest( d0, d1, d2 );
	    double smallest = findSmallest( d0, d1, d2 );
	    
	    double diff = (smallest * .10);
	    if ( ( largest - smallest ) < diff ){ 
	        //printf("Largest: %.2f, Smallest: %.2f, pts[%.2f, %.2f, %.2f]\n", largest, smallest, d0, d1, d2);
	        return true;
	    } else
	        return false;
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
	 * Returns the number of triangles found by the thread.
	 * @return numFound integer
	 */
	public int getFound(){
		return this.numFound;
	}
	/**
	 * This is where all the work for the thread gets executed.
	 */
	@Override
	public void run() {
		double d0, d1, d2;
		for(int i = startIndex; i < endIndex; i++){
			for(int j = i+1; j <= OGsize - 2; j++){
				for( int k = j+1; k <= OGsize - 1; k++){
					d0 = pList.get(i).distance(pList.get(j));
					d1 = pList.get(j).distance(pList.get(k));
					d2 = pList.get(i).distance(pList.get(k));
					
					if(check(d0,d1,d2))
						numFound++;
				}
			}
		}
		
	}
}
/**
 * Semilateral is the primary class in this system, in that it accepts and reads user input and files.
 * 
 * The user will indicate a specific, probably small, number (int) of threads to create and use during execution.
 * 
 * Once this value is obtained from the command line, the program then creates that many Threads.
 * 
 * After these threads are created, Semilateral then divides up the number of points from the input file relatively
 * 	evenly among the workers and lets them do the work. They will be assigned a specific portion of the array to analyze.
 * 
 * Semilateral will wait for each of the workers to finish execution, 
 * 	and then add up the total from their collective memory and report it.
 * Hopefully, this process speeds up in accordance with the number of worker threads created. 
 * 
 * @author Ryan Buchanan
 *
 */
public class Semilateral {
	//Using an arraylist is a lot easier because it resizes itself.
	public static ArrayList<Point> pointList;
	public static int total = 0;
	/**
	* Main method used in collecting input.
	*@param args[] 
	*/
	public static void main(String[] args){
		if(args.length == 0){
			System.out.println("Invalid number of arguments. ");
			System.exit(1);
		} 
        Scanner in = new Scanner(System.in);
		pointList = new ArrayList<Point>();
		//Reading from the file
		//Creating points
		//Storing the points in the array list
		//File reader
		//FileReader fileReader = new FileReader(in.nextLine());
		//BufferedReader buff = new BufferedReader(fileReader);
		//Scanner pointGet = new Scanner(buff);
		//Add to the list
		while(in.hasNextLine()){
			int p1 = in.nextInt();
			int p2 = in.nextInt();           	
			pointList.add(new Point(p1, p2));
			//in.nextLine();
		}
        in.close();
		//Create and start the threads.
		int numThreads = Integer.parseInt(args[0]);
		int finalI = 0;
		//This is to determine, whether or not we have an odd number of indicies.
		//This makes a difference depending on the number of workers we have.
		//Now I can make sure things devide relatively evenly among the workers and the indicies.
		if( pointList.size() % 2 == 0 && numThreads != 1){
			if(numThreads % 2 == 0){
				finalI = 0;
			} else
				finalI = 1;
		} else if( pointList.size() % 2 != 0 && numThreads != 1){
			if(numThreads % 2 == 0){
				finalI = 1;
			} else
				finalI = 0;
		}		
		semiThread thrds[] = new semiThread[numThreads];		
		for(int i = 0; i < numThreads; i++){
	        int base, cap;
	         //They will add the index that we subtracted above in order to get equal distribution. 
	         //This way, we go all the way to the end, and dont leave anything off.
	         if ( finalI == 1 ){
	             base = (( pointList.size() - 1) / numThreads) * i;   
	             cap = (( pointList.size() - 1) / numThreads) * (i+1);
	             if ( i == numThreads - 1 ){
	               cap++;
	             }
	         } else {
	             base = ( pointList.size() / numThreads) * i;   
	             cap = ( pointList.size() / numThreads) * (i+1);
	        }    
			thrds[i] = new semiThread(pointList, base, cap);
			thrds[i].start();
		}
		
	    try {
			for(int i = 0; i < numThreads; i++){
				thrds[i].join();
				total += thrds[i].getFound();
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	    
	    System.out.println("Triangles: " + total);
	}
}