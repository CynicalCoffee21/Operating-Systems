public class Interleaving {
  /** Thread to print ab. */

  static class ABthread implements Runnable{
      public void run(){
          System.out.print("a");
         System.out.print("b");
      }
  }

  /** Thread to print cd. */

  static class CDthread implements Runnable{
      public void run(){
          System.out.print("c");
          System.out.print("d");

      }
  }

  /** Thread to print ef. */

  static class EFthread implements Runnable{
      public void run(){
          System.out.print("e");
          System.out.print("f");

      }
  }

  public static void main( String[] args ) {
    // The three threads we make.
    Thread tab, tcd, tef;
  
    // A bunch of times.
    for ( int i = 0; i < 50000; i++ ) {
      //Clear the string builder for another run.
      //SharedString.delete(0,5);
      // Make one of each type of threads.      
      tab = new Thread(new ABthread());
      tcd = new Thread(new CDthread());
      tef = new Thread(new EFthread());
    
      // Start them all.
      tab.start();
      tcd.start();
      tef.start();

      // Join with our three threads.
      try{
          tab.join();
          tcd.join();
          tef.join();
      }catch(InterruptedException e){
         System.out.println("Unable to join threads.");
      }
      // Print a newline to end the line they just printed.
      System.out.println();
    }
  }
}