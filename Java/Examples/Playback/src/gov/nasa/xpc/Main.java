package gov.nasa.xpc;

import java.io.*;
import java.util.Scanner;

public class Main
{

    public static void main(String[] args)
    {
        String[] mainOpts = new String[] { "Record X-Plane", "Playback File", "Exit" };

        System.out.println("X-Plane Connect Playback Example  [Version 1.2.0.0]\n");
        System.out.println("(c) 2013-2015 United States Government as represented by the Administrator\n");
        System.out.println("of the National Aeronautics and Space Administration. All Rights Reserved.\n");
        while(true)
        {
            int result = displayMenu("What would you like to do?", mainOpts);
        }
//        char* mainOpts[3] =
//            {
//                    "Record X-Plane",
//                    "Playback File",
//                    "Exit"
//            };
//        char path[256] = { 0 };
//
//        displayStart("1.2.0.0");
//        while (1)
//        {
//            switch (displayMenu("What would you like to do?", mainOpts, 3))
//            {
//            case 1:
//            {
//                getString("Enter save file path", path);
//                int interval = getInt("Enter interval between frames (milliseconds)");
//                int duration = getInt("Enter duration to record for (seconds)");
//
//                record(path, interval, duration);
//                break;
//            }
//            case 2:
//            {
//                getString("Enter path to saved playback file", path);
//                int interval = getInt("Enter interval between frames (milliseconds)");
//
//                playback(path, interval);
//                break;
//            }
//            case 3:
//                displayMsg("Exiting.");
//                return 0;
//            default:
//                displayMsg("Unrecognized option.");
//                break;
//            }
//        }
//
//        return 0;

    }

    private static int displayMenu(String title, String[] opts) throws IOException
    {
        System.out.println();
        System.out.println("+---------------------------------------------- +\n");
        System.out.println(String.format("|  %1$-42s   |\n", title));
        System.out.println("+---------------------------------------------- +\n");
        for(int i = 0; i < opts.length; ++i)
        {
            System.out.println(String.format("| %1$2d. %2$-40s  |\n", i + 1, opts[i]));

        }
        System.out.println("+---------------------------------------------- +\n");
        System.out.print("Please select an option: ");
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
                    float[] posi = xpc.getPOSI(0);
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
            System.out.println("Starting playback...");
            try (XPlaneConnect xpc = new XPlaneConnect())
            {
                while(reader.hasNextLine())
                {
                    float[] posi = new float[7];
                    for (int i = 0; i < 7; ++i)
                    {
                        posi[i] = reader.nextFloat();
                        reader.skip(", ");
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
