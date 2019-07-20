package gov.nasa.xpc.ex;

import gov.nasa.xpc.XPlaneConnect;
import gov.nasa.xpc.discovery.XPlaneConnectDiscovery;

import java.io.IOException;
import java.net.SocketException;



public class DiscoveryConnectionExample {

    public static void main(String[] args) throws IOException {

        XPlaneConnectDiscovery discovery = new XPlaneConnectDiscovery();
        discovery.start(xpc -> {
             
            sendDref(xpc);
        });
        System.out.println("Example done");
    }

    static void sendDref(XPlaneConnect xpc) {
        System.out.println("Sending DREF");
        try {
            xpc.getDREF("sim/test/test_float");

        } catch (SocketException e) {
            System.out.println("Unable to set up the connection. (Error message was '" + e.getMessage() + "'.)");
        } catch (IOException e) {
            System.out.println("Something went wrong with one of the commands. (Error message was '" + e.getMessage() + "'.)");
        }
        System.out.println("Sending DREF done");
    }
}
