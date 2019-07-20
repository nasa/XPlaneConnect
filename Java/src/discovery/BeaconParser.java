package gov.nasa.xpc.discovery;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Parser class that parses the UDP discovery BECN packet. The format of the packet is
 * - 4 bytes: BECN
 * - 1 byte: 0
 * - 2 bytes: XPlaneConnect server port e.g. '49009'
 * - 4 bytes: X-Plane version e.g. '11260'
 * - null terminated string: XPlaneConnect plugin version e.g. '1.3-rc.1'
 */
public class BeaconParser {

    private static String BECN = "BECN";

    private static int XPC_PORT_OFFSET = BECN.length() + 1;
    private static int XPC_PORT_LEN = 2;
    private static int XPC_VERSION_OFFSET = XPC_PORT_OFFSET + XPC_PORT_LEN;
    private static int XPC_VERSION_LEN = 4;
    private static int XPC_PLUGIN_VERSION_OFFSET = XPC_VERSION_OFFSET + XPC_VERSION_LEN;


    public Beacon readBCN(DatagramPacket packet) throws IOException {
        if (packet.getLength() < BECN.length()) {
            throw new IOException("BECN response too short");
        }

        byte[] data = packet.getData();

        String command = new String(data, 0, BECN.length());
        if (!command.equals(BECN)) {
            throw new IOException("Expected " + BECN + " got '" + command + "'");
        }

        InetAddress address = packet.getAddress();
        ByteBuffer bb = ByteBuffer.wrap(data);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        // 2 bytes: port as short + converted to an int
        int port = bb.getShort(XPC_PORT_OFFSET) & 0xffff;

        // 4 bytes: x plane version as int
        int version = bb.getInt(XPC_VERSION_OFFSET);

        // plugin version
        String pluginVersion = new String(data, XPC_PLUGIN_VERSION_OFFSET, packet.getLength() - XPC_PLUGIN_VERSION_OFFSET);

        return new Beacon(address, port, pluginVersion.trim(), version);
    }
}