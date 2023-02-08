package gov.nasa.xpc.ex;

import gov.nasa.xpc.discovery.XPlaneConnectDiscovery;

import java.io.IOException;


/**
 * XPlaneConnect discovery example that prints continuously each BECN packet
 */
public class SimpleDiscoveryExample {

    public static void main(String[] args) throws IOException {
XPlaneConnectDiscovery discovery = new XPlaneConnectDiscovery();
discovery.onBeaconReceived(beacon -> {
    System.out.println("Discovered XPlaneConnect plugin:");
    System.out.println("Plugin version: " + beacon.getPluginVersion());
    System.out.println("X-Plane version: " + beacon.getXplaneVersion());
    System.out.println("Address: " + beacon.getXplaneAddress().getHostAddress() + ":" + beacon.getPluginPort());
});
discovery.start();
    }
}
