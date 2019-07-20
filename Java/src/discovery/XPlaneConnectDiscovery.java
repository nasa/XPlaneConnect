package gov.nasa.xpc.discovery;

import gov.nasa.xpc.XPlaneConnect;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.SocketException;


/**
 * The XPlaneConnectDiscovery joins a UDP multi cast group and listens for BECN packets published by
 * XPlaneConnect server plugin. It allows to clients to discover XPlane instances and be sure that the XPlanePlugin is
 * installed.
 */
public class XPlaneConnectDiscovery implements AutoCloseable {

    private static int DEFAULT_PORT = 49710;
    private static String DEFAULT_ADDRESS = "239.255.1.1";

    private BeaconReceivedListener mListener;

    private MulticastSocket socket = null;
    private byte[] buf = new byte[256];

    private int mPort;
    private String mAddress;
    private BeaconParser parser = new BeaconParser();

    private XPlaneConnectDiscovery(int port, String address) {
        mPort = port;
        mAddress = address;
    }

    public XPlaneConnectDiscovery() {
        this(DEFAULT_PORT, DEFAULT_ADDRESS);
    }

    public void start(DiscoveryConnectionCallback callback) throws IOException {
        onBeaconReceived(beacon -> {
            this.close();

            try {
                XPlaneConnect xpc = new XPlaneConnect(beacon);
                callback.onConnectionEstablished(xpc);
            } catch (SocketException e) {
                System.err.println("Could not connect to a discovered XplaneConnect " +beacon+ ": " + e.getMessage());
            }
        });
        start();

    }

    public void start() throws IOException {
        System.out.println("Starting XPlane Connect discovery");
        socket = new MulticastSocket(mPort);
        InetAddress group = InetAddress.getByName(mAddress);
        socket.joinGroup(group);
        while (socket != null) {
            DatagramPacket packet = new DatagramPacket(buf, buf.length);
            socket.receive(packet);
            if (mListener == null) {
                continue;
            }
            try {
                Beacon beacon = parser.readBCN(packet);
                mListener.onBeaconReceived(beacon);
            } catch (IOException ex) {
                System.err.println("Received packet on discovery group but could not parse it: " + ex);
            }
        }

        close();
        System.out.println("XPlane Connect discovery ended");
    }


    public void onBeaconReceived(BeaconReceivedListener mListener) {
        this.mListener = mListener;
    }

    @Override
    public void close() {
        if (socket != null) {
            socket.close();
            socket = null;
        }
    }

    public void stop() {
        close();
    }
}
