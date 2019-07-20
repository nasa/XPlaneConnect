package gov.nasa.xpc.discovery;

import java.net.InetAddress;


public class Beacon {

    private InetAddress xplaneAddress;
    private int pluginPort;
    private String pluginVersion;
    private int xPlaneVersion;


    public Beacon(InetAddress xplaneAddress, int pluginPort, String pluginVersion, int xPlaneVersion) {
        this.xplaneAddress = xplaneAddress;
        this.pluginPort = pluginPort;
        this.pluginVersion = pluginVersion;
        this.xPlaneVersion = xPlaneVersion;
    }

    public String getHost() {
        return xplaneAddress.getHostAddress();
    }

    public String getPluginVersion() {
        return pluginVersion;
    }

    public String getXplaneVersion() {
        return String.format("%.2f", xPlaneVersion / 100.0);
    }

    public int getPluginPort() {
        return pluginPort;
    }

    public InetAddress getXplaneAddress() {
        return xplaneAddress;
    }

    @Override
    public String toString() {
        return "host: " + getHost() + ":" + getPluginPort() +" version: " + getPluginVersion() + " X-Plane Version: " + getXplaneVersion();
    }
}
