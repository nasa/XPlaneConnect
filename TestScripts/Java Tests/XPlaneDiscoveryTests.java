package gov.nasa.xpc.test;

import static org.junit.Assert.*;

import gov.nasa.xpc.discovery.Beacon;
import gov.nasa.xpc.discovery.BeaconParser;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.net.DatagramPacket;


public class XPlaneDiscoveryTests {

    private BeaconParser parser;
    private DatagramPacket packet;

    @Before
    public void setup() {
        parser = new BeaconParser();
        byte[] bytes = HexHelper.decodeHexString("4245434E0071BFFC2B0000312E332D72632E310000000000000000");
        packet = new DatagramPacket(bytes, 0, bytes.length);
    }

    @Test
    public void readBeaconParsesPort() throws IOException {
        Beacon beacon = parser.readBCN(packet);
        assertEquals(beacon.getPluginPort(), 49009);
    }

    @Test
    public void readBeaconParsesXplaneVersion() throws IOException {
        Beacon beacon = parser.readBCN(packet);
        assertEquals(beacon.getXplaneVersion(), "112.60");

    }

    @Test
    public void readBeaconParsesPluginVersion() throws IOException {
        Beacon beacon = parser.readBCN(packet);
        assertEquals(beacon.getPluginVersion(), "1.3-rc.1");
    }
}

class HexHelper {
    static byte[] decodeHexString(String hexString) {
        if (hexString.length() % 2 == 1) {
            throw new IllegalArgumentException("Invalid hexadecimal String supplied.");
        }

        byte[] bytes = new byte[hexString.length() / 2];
        for (int i = 0; i < hexString.length(); i += 2) {
            bytes[i / 2] = hexToByte(hexString.substring(i, i + 2));
        }
        return bytes;
    }

    private static byte hexToByte(String hexString) {
        int firstDigit = toDigit(hexString.charAt(0));
        int secondDigit = toDigit(hexString.charAt(1));
        return (byte) ((firstDigit << 4) + secondDigit);
    }

    private static int toDigit(char hexChar) {
        int digit = Character.digit(hexChar, 16);
        if (digit == -1) {
            throw new IllegalArgumentException(
                    "Invalid Hexadecimal Character: " + hexChar);
        }
        return digit;
    }
}
