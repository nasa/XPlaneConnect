package gov.nasa.xpc.ex;

import gov.nasa.xpc.XPlaneConnect;
import java.io.IOException;
import java.net.SocketException;
import java.util.Arrays;

/**
 * An example program demonstrating the basic features of the X-Plane Connect toolbox.
 *
 * @author Jason Watkins
 * @version 1.0
 * @since 2015-04-06
 */
public class Main
{
    public static void main(String[] args)
    {
        System.out.println("X-Plane Connect example program");
        System.out.println("Setting up simulation...");
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            // Ensure connection established.
            xpc.getDREF("sim/test/test_float");

            System.out.println("Setting player aircraft position");
            double[] posi = new double[] {37.524, -122.06899, 2500, 0, 0, 0, 1};
            xpc.sendPOSI(posi);

            System.out.println("Setting another aircraft position");
            posi[0] = 37.52465;
            posi[4] = 20;
            xpc.sendPOSI(posi, 1);

            System.out.println("Setting rates");
            float[][] data = new float[3][9];
            for(float[] row : data)
            {
                Arrays.fill(row, -998);
            }
            data[0][0] = 18; //Alpha
            data[0][1] = 0;
            data[0][3] = 0;

            data[1][0] = 3; //Velocity
            data[1][1] = 130;
            data[1][2] = 130;
            data[1][3] = 130;
            data[1][4] = 130;

            data[2][0] = 16; //PQR
            data[2][1] = 0;
            data[2][2] = 0;
            data[2][3] = 0;

            xpc.sendDATA(data);

            System.out.println("Setting controls");
            float[] ctrl = new float[4];
            ctrl[3] = 0.8F;
            xpc.sendCTRL(ctrl);

            System.out.println("Pausing sim");
            xpc.pauseSim(true);
            try { Thread.sleep(5000); } catch (InterruptedException ex) {}
            System.out.println("Un-pausing");
            xpc.pauseSim(false);

            //Let sim run for 10 seconds
            try { Thread.sleep(10000); } catch (InterruptedException ex) {}

            System.out.println("Stowing landing gear");
            xpc.sendDREF("sim/cockpit/switches/gear_handle_status", 1);

            //Let sim run for 10 seconds
            try { Thread.sleep(10000); } catch (InterruptedException ex) {}

            System.out.println("Checking gear and sim status");
            String[] drefs = new String[]
            {
                "sim/cockpit/switches/gear_handle_status",
                "sim/operation/override/override_planepath"
            };
            float[][] results = xpc.getDREFs(drefs);

            if(results[0][0] == 1)
            {
                System.out.println("Gear stowed.");
            }
            else
            {
                System.out.println("Error stowing gear");
            }
            System.out.println("Example complete");
        }
        catch (SocketException ex)
        {
            System.out.println("Unable to set up the connection. (Error message was '" + ex.getMessage() + "'.)");
        }
        catch (IOException ex)
        {
            System.out.println("Something went wrong with one of the commands. (Error message was '" + ex.getMessage() + "'.)");
        }
        System.out.println("Exiting");
    }
}
