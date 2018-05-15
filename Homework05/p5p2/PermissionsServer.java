import java.io.*;
import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.util.*;

/** A simple class implementing a server based on a datagram socket. */
public class PermissionsServer {
  /** Port number used by the server.  */
  public static final int SERVER_PORT = 26044;
  private static ArrayList<String> uList;
  private static ArrayList<String> oList;
  private static ArrayList<String> table;
  public static void main( String[] args ) {
	try{
	    File usersFile = new File("users.txt");
	    Scanner uScan = new Scanner(usersFile);
	    uList = new ArrayList<String>();
	    /*
	     * Scan and store the user file.
	     */
	    while(uScan.hasNextLine()){
	      uList.add(uScan.nextLine());
	    }
	    File objectsFile = new File("objects.txt");
	    Scanner oScan = new Scanner(objectsFile);
	    oList = new ArrayList<String>();
	    /*
	     * Scan and store the object file
	     */
	    while(oScan.hasNextLine()){
	      oList.add(oScan.nextLine());
	    }
	    File tableFile = new File("table.txt");
	    Scanner tScan = new Scanner(tableFile);
	    table = new ArrayList<String>();
	    /*
	     * Scan and store the table file
	     */
	    while(tScan.hasNextLine()){
	      int user = tScan.nextInt();
	      int obj = tScan.nextInt();
	      String opp = tScan.next();
	      String perm = uList.get(user) + " " + oList.get(obj) + " " + opp;
        //System.out.println(perm);
	      table.add(perm);
        tScan.nextLine();
	    }
      //System.out.println(table.size());
	    uScan.close();
	    oScan.close();
	    tScan.close();
	} catch(IOException e){
      e.printStackTrace();
	}
    /*
    * Most of the below is taken straight from the UDPServer code.
    * While most of the above is my code.
    * I made some changes to the code from the UDPServer to fit the current
    * program and I added a search through the list. 
    */
    DatagramSocket sock = null;
    try {
      sock = new DatagramSocket( SERVER_PORT );
    } catch( IOException e ){
      System.err.println( "Can't create socket: " + e );
      System.exit( -1 );
    }
    byte[] recvBuffer = new byte [ 1024 ];
    DatagramPacket recvPacket = new DatagramPacket( recvBuffer, recvBuffer.length );
    try{
      while( true ){
        sock.receive( recvPacket );
        String str = new String( recvBuffer, 0, recvPacket.getLength() );
        String response = null;
        /*
        * Search the table for a matching permission/object/user combonation.
        */
        for(int i = 0; i < table.size(); i++){
          //If match is found, indicate that
          if(str.equals(table.get(i)))
            response = "Access Granted";
        }
        //If no match was found, indicate that
        if(response == null)
          response = "Access Denied";
        // Send
        byte[] sendBuffer = response.getBytes();
        DatagramPacket sendPacket = new DatagramPacket( sendBuffer, sendBuffer.length,
                                                        recvPacket.getAddress(),
                                                        recvPacket.getPort() );
        sock.send( sendPacket );
      }
    } catch( IOException e ){
      System.err.println( "Error communicating with a client." );
    }
  }
}
