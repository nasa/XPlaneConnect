package gov.nasa.xpc;

import com.sun.javafx.scene.EnteredExitedHandler;

import java.io.*;
import java.util.Scanner;

public class Main
{

    public static void main(String[] args) throws IOException
    {
        String[] mainOpts = new String[] { "Record X-Plane", "Playback File", "Exit" };

        System.out.println("X-Plane Connect Playback Example  [Version 1.3-rc.1]");
        System.out.println("(c) 2013-2015 United States Government as represented by the Administrator");
        System.out.println("of the National Aeronautics and Space Administration. All Rights Reserved.");
        while(true)
        {
            int result = displayMenu("What would you like to do?", mainOpts);
            switch(result)
            {
            case 1:
            {
                String path = getString("Enter a save file path: ");
                int interval = getInt("Enter interval between frames (milliseconds): ");
                int duration = getInt("Enter duration to record for (seconds): ");

                record(path, interval, duration);
                break;
            }
            case 2:
            {
                String path = getString("Enter path to saved playback file: ");
                int interval = getInt("Enter interval between frames (milliseconds): ");

                playback(path, interval);
                break;
            }
            case 3:
            {
                System.out.println("Exiting.");
                return;
            }
            default:
            {
                System.out.println("Unrecognized menu option.");
                break;
            }
            }
        }
    }

    private static int displayMenu(String title, String[] opts) throws IOException
    {
        System.out.println();
        System.out.println("+---------------------------------------------- +");
        System.out.println(String.format("|  %1$-42s   |", title));
        System.out.println("+---------------------------------------------- +");
        for(int i = 0; i < opts.length; ++i)
        {
            System.out.println(String.format("| %1$2d. %2$-40s  |", i + 1, opts[i]));

        }
        System.out.println("+---------------------------------------------- +");
        return getInt("Please select an option: ");
    }

    private static String getString(String prompt) throws IOException
    {
        System.out.print(prompt);

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        return reader.readLine();
    }

    private static int getInt(String prompt) throws IOException
    {
        System.out.print(prompt);

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        String result = reader.readLine();

        return Integer.parseInt(result);
    }

    public static void record(String path, int interval, int duration) throws IOException
    {
        int count = duration * 1000 / interval;
        if (count < 1)
        {
            throw new IllegalArgumentException("duration is less than one interval.");
        }

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(path)))
        {
            System.out.println("Recording...");
            try (XPlaneConnect xpc = new XPlaneConnect())
            {
                for (int i = 0; i < count; ++i)
                {
                    float[] posi = xpc.getPOSI(0); // FIXME: change this to 64-bit double
                    writer.write(String.format("%1$f, %2$f, %3$f, %4$f, %5$f, %6$f, %7$f\n",
                            posi[0], posi[1], posi[2], posi[3], posi[4], posi[5], posi[6]));
                    try
                    {
                        Thread.sleep(interval);
                    }
                    catch (InterruptedException ex)
                    {
                    }
                }
            }
            System.out.println("Recording Complete");
        }
    }

    public static void playback(String path, int interval) throws IOException
    {
        try(Scanner reader = new Scanner(new FileReader(path)))
        {
            reader.useDelimiter("[\\s\\r\\n,]+");
            System.out.println("Starting playback...");
            try (XPlaneConnect xpc = new XPlaneConnect())
            {
                while(reader.hasNextLine())
                {
                    double[] posi = new double[7];
                    for (int i = 0; i < 7; ++i)
                    {
                        String s = reader.next();
                        posi[i] = Double.parseDouble(s);
                    }
                    reader.nextLine();
                    xpc.sendPOSI(posi);
                    try
                    {
                        Thread.sleep(interval);
                    }
                    catch (InterruptedException ex)
                    {
                    }
                }
            }
        }
    }
}
