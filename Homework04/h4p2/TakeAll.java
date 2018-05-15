import java.util.Random;

public class TakeAll {
  /** To tell all the chefs when they can quit running. */
  private static boolean running = true;

  /** Superclass for all chefs.  Contains methods to cook and rest and
      keeps a record of how many dishes were prepared. */
  private static class Chef extends Thread {
    /** Number of dishes prepared by this chef. */
    private int dishCount = 0;

    /** Source of randomness for this chef. */
    private Random rand = new Random();

    /** Called after the chef has locked all the required appliances and is
        ready to cook for about the given number of milliseconds. */
    protected void cook( int duration ) {
      System.out.printf( "%s is cooking\n", getClass().getSimpleName() );
      try {
        // Wait for a while (pretend to be cooking)
        Thread.sleep( rand.nextInt( duration / 2 ) + duration / 2 );
      } catch ( InterruptedException e ) {
      }
      dishCount++;
    }

    /** Called between dishes, to let the chef rest before cooking another dish. */
    protected void rest( int duration ) {
      System.out.printf( "%s is resting\n", getClass().getSimpleName() );
      try {
        // Wait for a while (pretend to be resting)
        Thread.sleep( rand.nextInt( duration / 2 ) + duration / 2 );
      } catch ( InterruptedException e ) {
      }
    }
  }

  // An object representing the lock on each appliance.
  // Locking the needed objects before cooking prevents two
  // chefs from trying to use the same appliance at the same time.
  private static boolean grill = true;
  private static boolean oven = true;
  private static boolean microwave = true;
  private static boolean mixer = true;
  private static boolean blender = true;
  private static boolean coffeeMaker = true;
  private static boolean fryer = true;
  private static boolean griddle = true;

  private static Object getApp = new Object();
/*
  private static final Object grillLock = new Object();
  private static final Object ovenLock = new Object();
  private static final Object microwaveLock = new Object();
  private static final Object mixerLock = new Object();
  private static final Object blenderLock = new Object();
  private static final Object coffeeMakerLock = new Object();
  private static final Object fryerLock = new Object();
  private static final Object griddleLock = new Object();
*/
  /** Ernest is a chef needing 15 milliseconds to prepare a dish. */
  private static class Ernest extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
            while( !fryer && !coffeeMaker && !grill )
              getApp.wait();
            fryer = false;
            coffeeMaker = false;
            grill = false;
          }catch(InterruptedException e){
              e.printStackTrace();
          }
        }
        cook( 15 );
        synchronized( getApp ){
          coffeeMaker = true;
          grill = true;
          fryer = true;
          getApp.notifyAll();
        }
        rest( 25 );
      }
    }
  }

  /** Jess is a chef needing 105 milliseconds to prepare a dish. */
  private static class Jess extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
              while( !griddle && !fryer )
                getApp.wait();
              griddle = false;
              fryer = false;
            }catch(InterruptedException e){
                e.printStackTrace();
            }
        }
        cook( 105 );

        synchronized( getApp ){
          griddle = true;
          fryer = true;
          getApp.notifyAll();
        }
        rest( 25 );
      }
    }
  }

  /** Chantal is a chef needing 30 milliseconds to prepare a dish. */
  private static class Chantal extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
              while(!microwave && !oven)
                getApp.wait();
            }catch(InterruptedException e){
                e.printStackTrace();
            }
        }
        cook( 30 );
        synchronized( getApp ){
          microwave = true;
          oven = true;
          getApp.notifyAll();
        }
        rest( 25 );
      }
    }
  }

  /** Fabiola is a chef needing 30 milliseconds to prepare a dish. */
  private static class Fabiola extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
            while(!microwave && !oven)
              getApp.wait();
            }catch(InterruptedException e){
                e.printStackTrace();
            }
        }
        cook( 30 );

        synchronized( getApp ){
          microwave = true;
          oven = true;
          getApp.notifyAll();
        }
        rest( 25 );
      }
    }
  }

  /** Chas is a chef needing 75 milliseconds to prepare a dish. */
  private static class Chas extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
              while( !griddle && !blender )
                getApp.wait();
            }catch(InterruptedException e){
                e.printStackTrace();
            }
        }
        cook( 75 );
        synchronized( getApp ){
          griddle = true;
          blender = true;
          getApp.notifyAll();
        }

        rest( 25 );
      }
    }
  }

  /** Cora is a chef needing 45 milliseconds to prepare a dish. */
  private static class Cora extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
              while( !mixer && !microwave )
                getApp.wait();
            }catch(InterruptedException e){
                e.printStackTrace();
            }
        }
            cook( 45 );
            synchronized( getApp ){
              mixer = true;
              microwave = true;
              getApp.notifyAll();
            }

        rest( 25 );
      }
    }
  }

  /** Hugh is a chef needing 15 milliseconds to prepare a dish. */
  private static class Hugh extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
              while( !fryer && !oven && !grill )
                getApp.wait();
            }catch(InterruptedException e){
                e.printStackTrace();
            }
        }
        cook( 15 );
        synchronized( getApp ){
          fryer = true;
          oven = true;
          grill = true;
          getApp.notifyAll();
        }

        rest( 25 );
      }
    }
  }

  /** Maritza is a chef needing 60 milliseconds to prepare a dish. */
  private static class Maritza extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
              while( !blender && !coffeeMaker && !mixer )
                getApp.wait();
            }catch(InterruptedException e){
                e.printStackTrace();
            }
        }
        cook( 60 );
        synchronized( getApp ){
          blender = true;
          coffeeMaker = true;
          mixer = true;
          getApp.notifyAll();
        }

        rest( 25 );
      }
    }
  }

  /** Jerrod is a chef needing 60 milliseconds to prepare a dish. */
  private static class Jerrod extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
            while( !blender && !coffeeMaker && !mixer )
              getApp.wait();
            }catch(InterruptedException e){
                e.printStackTrace();
            }
        }
        cook( 60 );
        synchronized( getApp ){
          blender = true;
          coffeeMaker = true;
          mixer = true;
          getApp.notifyAll();
        }

        rest( 25 );
      }
    }
  }

  /** Ida is a chef needing 15 milliseconds to prepare a dish. */
  private static class Ida extends Chef {
    public void run() {
      while ( running ) {
        // Get the appliances this chef uses.
        synchronized( getApp ){
          try{
              while( !griddle && !grill )
                getApp.wait();
            }catch(InterruptedException e){
                e.printStackTrace();
            }
        }
        cook( 15 );
        synchronized( getApp ){
          griddle = true;
          grill = true;
          getApp.notifyAll();
        }

        rest( 25 );
      }
    }
  }

  public static void main( String[] args ) throws InterruptedException {
    // Make a thread for each of our chefs.
    Chef chefList[] = {
      new Ernest(),
      new Jess(),
      new Chantal(),
      new Fabiola(),
      new Chas(),
      new Cora(),
      new Hugh(),
      new Maritza(),
      new Jerrod(),
      new Ida(),
    };

    // Start running all our chefs.
    for ( int i = 0; i < chefList.length; i++ )
      chefList[ i ].start();

    // Let the chefs cook for a while, then ask them to stop.
    Thread.sleep( 10000 );
    running = false;

    // Wait for all our chefs to finish, and collect up how much
    // cooking was done.
    int total = 0;
    for ( int i = 0; i < chefList.length; i++ ) {
      chefList[ i ].join();
      System.out.printf( "%s cooked %d dishes\n",
                         chefList[ i ].getClass().getSimpleName(),
                         chefList[ i ].dishCount );
      total += chefList[ i ].dishCount;
    }
    System.out.printf( "Total dishes cooked: %d\n", total );
  }
}
