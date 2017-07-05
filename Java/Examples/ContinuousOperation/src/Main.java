package gov.nasa.xpc.ex;

import gov.nasa.xpc.XPlaneConnect;

import java.io.IOException;

/**
 * An example program demonstrating the use of X-PlaneConnect in a continuous loop.
 *
 * @author Jason Watkins
 * @version 1.0
 * @since 2015-06-19
 */
public class Main
{
    public static void main(String[] args)
    {
        try (XPlaneConnect xpc = new XPlaneConnect())
        {
            int aircraft = 0;
            while(true)
            {
                float[] posi = xpc.getPOSI(aircraft); // FIXME: change this to 64-bit double
                float[] ctrl = xpc.getCTRL(aircraft);

                System.out.format("Loc: (%4f, %4f, %4f) Aileron:%2f Elevator:%2f Rudder:%2f\n",
                        posi[0], posi[1], posi[2], ctrl[1], ctrl[0], ctrl[2]);

                try
                {
                    Thread.sleep(100);
                }
                catch (InterruptedException ex) {}

                if(System.in.available() > 0)
                {
                    break;
                }
            }
        }
        catch(IOException ex)
        {
            System.out.println("Error:");
            System.out.println(ex.getMessage());
        }
    }
}
