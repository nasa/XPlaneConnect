package gov.nasa.xpc.test;

import gov.nasa.xpc.XPlaneConnect;

import static org.junit.Assert.*;
import org.junit.Test;

import java.io.IOException;
import java.net.SocketException;
import java.net.UnknownHostException;

public class XPlaneConnectTest
{
    @Test
    public void constructorTest()
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            assertNotNull(xpc);
        }
        catch (SocketException ex)
        {
            fail();
        }
        try(XPlaneConnect xpc = new XPlaneConnect(200))
        {
            assertNotNull(xpc);
        }
        catch (SocketException ex)
        {
            fail();
        }
        try(XPlaneConnect xpc = new XPlaneConnect(49007, "127.0.0.1", 49009))
        {
            assertNotNull(xpc);
        }
        catch (SocketException | UnknownHostException ex)
        {
            fail();
        }
        try(XPlaneConnect xpc = new XPlaneConnect(49007, "127.0.0.1", 49009, 200))
        {
            assertNotNull(xpc);
        }
        catch (SocketException | UnknownHostException ex)
        {
            fail();
        }
    }

    @Test
    public void testGetRecvPort() throws SocketException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            assertEquals(49008, xpc.getRecvPort());
        }
    }

    @Test
    public void testGetXPlanePort() throws SocketException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            assertEquals(49009, xpc.getXPlanePort());
        }
    }

    @Test
    public void testGetXPlaneAddr() throws SocketException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            assertEquals("127.0.0.1", xpc.getXPlaneAddr());
        }
    }

    @Test
    public void testSetXPlaneAddr() throws SocketException, UnknownHostException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.setXplaneAddr("10.10.0.100");
            assertEquals("10.10.0.100", xpc.getXPlaneAddr());
        }
    }

    @Test
    public void constructorTest_SocketAlreadyBound() throws SocketException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            try(XPlaneConnect xpc2 = new XPlaneConnect())
            {
                fail();
            }
            catch (SocketException ex) {}
        }
        catch (SocketException ex)
        {
            fail();
        }
    }

    @Test(expected = UnknownHostException.class)
    public void constructorTest_InvalidHost() throws UnknownHostException
    {
        try(XPlaneConnect xpc = new XPlaneConnect(49007, "notarealhost", 49009))
        {

        }
        catch (SocketException ex)
        {
            fail();
        }
        fail();
    }

    @Test
    public void testRequestDREF() throws IOException
    {
        String dref = "sim/cockpit/switches/gear_handle_status";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            float[] result = xpc.requestDREF(dref);
            assertEquals(1, result.length);
        }
    }

    @Test
    public void testRequestDREFs() throws IOException
    {
        String[] drefs =
        {
            "sim/cockpit/switches/gear_handle_status",
            "sim/cockpit2/switches/panel_brightness_ratio"
        };
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            float[][] result = xpc.requestDREFs(drefs);
            assertEquals(2, result.length);
            assertEquals(1, result[0].length);
            assertEquals(4, result[1].length);
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testRequestDREFs_NullArgument() throws IOException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.requestDREFs(null);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testRequestDREFs_EmptyArgument() throws IOException
    {
        String[] drefs = new String[0];
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.requestDREFs(drefs);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testRequestDREFs_TooManyDREFs() throws IOException
    {
        String[] drefs = new String[300];
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.requestDREFs(drefs);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testRequestDREFs_DREFTooLong() throws IOException
    {
        String longDREF = "sim/cockpit/switches/i/am/a/very/long/fake/dref/that/is/over/255/characters/./which/means/that/my/length/cant/be/encoded/in/the/single/byte/allocated/by/the/message/format/,/so/i/should/cause/an/exception/instead/./i/am/still/not/long/enough/./almost/there";
        String[] drefs = new String[]{longDREF};
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.requestDREFs(drefs);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testRequestDREF_DREFEmpty() throws IOException
    {
        String dref = "";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.requestDREF(dref);
            fail();
        }
    }

    @Test
    public void testPauseSim() throws IOException
    {
        String dref = "sim/operation/override/override_planepath";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.pauseSim(true);
            float[] result = xpc.requestDREF(dref);
            //assertEquals(1, result.length); //TODO: Why is this result 20 elements long in Java? (It's only one in MATLAB)
            assertEquals(1, result[0], 1e-4);

            xpc.pauseSim(false);
            result = xpc.requestDREF(dref);
            //assertEquals(1, result.length);
            assertEquals(0, result[0], 1e-4);
        }
    }

    @Test
    public void testSendTEXT() throws IOException
    {
        String msg = "XPlaneConnect Java message test.";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendTEXT(msg, 200, 400);
        }
    }

    @Test
    public void testSendTEXT_Multiline() throws IOException
    {
        String msg = "XPlaneConnect Java message test.\nNow with new lines!\rAnd another...\r\nAnd now a double.";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendTEXT(msg, 200, 400);
        }
    }

    @Test
    public void testSendTEXT_Long() throws IOException
    {
        String msg = "XPlaneConnect Java message test.\nNow with new lines!\rAnd another...\r\nAnd now a double\nAnd finally a really long line because that seemed to break things.";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendTEXT(msg, 200, 400);
        }
    }

    @Test
    public void testSendDREF() throws IOException
    {
        String dref = "sim/cockpit/switches/gear_handle_status";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            float gearHandle = xpc.requestDREF(dref)[0];

            float value = gearHandle > 0.5 ? 0 : 1;
            xpc.sendDREF(dref, value);

            float result = xpc.requestDREF(dref)[0];
            assertEquals(value, result, 1e-4);
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendDREF_NullDREF() throws IOException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendDREF(null, 0);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendDREF_NullValue() throws IOException
    {
        String dref = "sim/cockpit/switches/gear_handle_status";

        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendDREF(dref, null);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendDREF_EmptyValue() throws IOException
    {
        String dref = "sim/cockpit/switches/gear_handle_status";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendDREF(dref, new float[0]);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendDREF_MessageTooLong() throws IOException
    {
        String dref = "sim/cockpit/switches/gear_handle_status";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendDREF(dref, new float[200]);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendDREF_DREFTooLong() throws IOException
    {
        String dref = "sim/cockpit/switches/i/am/a/very/long/fake/dref/that/is/over/255/characters/./which/means/that/my/length/cant/be/encoded/in/the/single/byte/allocated/by/the/message/format/,/so/i/should/cause/an/exception/instead/./i/am/still/not/long/enough/./almost/there";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendDREF(dref, 0);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendDREF_DREFEmpty() throws IOException
    {
        String dref = "";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendDREF(dref, 0);
            fail();
        }
    }

    @Test
    public void testSendCTRL() throws IOException
    {
        String[] drefs = {
                "sim/cockpit2/controls/yoke_pitch_ratio",
                "sim/cockpit2/controls/yoke_roll_ratio",
                "sim/cockpit2/controls/yoke_heading_ratio",
                "sim/flightmodel/engine/ENGN_thro",
                "sim/cockpit/switches/gear_handle_status",
                "sim/flightmodel/controls/flaprqst"
        };
        float[] ctrl = new float[] {0, 0, 1, 0.8F, 0, 1};
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendCTRL(ctrl);
            float[][] result = xpc.requestDREFs(drefs);
            if(result.length < ctrl.length)
            {
                fail();
            }
            for(int i = 0; i < 6; ++i)
            {
                assertEquals(ctrl[i], result[i][0], 1e-2);
            }
        }
    }

    @Test
    public void testSendCTRL_NPC() throws IOException
    {
        String[] drefs1 = {
                "sim/multiplayer/position/plane1_yolk_pitch",
                "sim/multiplayer/position/plane1_yolk_roll",
                "sim/multiplayer/position/plane1_yolk_yaw",
                "sim/multiplayer/position/plane1_throttle"
        };
        String[] drefs2 = {
                "sim/multiplayer/position/plane1_gear_deploy",
                "sim/multiplayer/position/plane1_flap_ratio"
        };
        float[] ctrl = new float[] {0, 0, 1, 0.8F, 0, 0.5F};
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendCTRL(ctrl, 1);
            float[][] result1 = xpc.requestDREFs(drefs1);
            float[][] result2 = xpc.requestDREFs(drefs2);
            if(result1.length != 4 || result2.length != 2)
            {
                fail();
            }
            for(int i = 0; i < 4; ++i)
            {
                assertEquals(ctrl[i], result1[i][0], 1e-2);
            }
            for(int i = 0; i < 2; ++i)
            {
                assertEquals(ctrl[i + 4], result2[i][0], 1e-2);
            }
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendCTRL_NullCtrl() throws IOException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendCTRL(null);
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendCTRL_LongCtrl() throws IOException
    {
        float[] ctrl = new float[] {0, 0, 1, 0.8F, 0, 1, -998};
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendCTRL(ctrl);
        }
    }

    @Test
    public void testSendPOSI() throws IOException
    {
        String[] drefs = {
            "sim/flightmodel/position/latitude",
            "sim/flightmodel/position/longitude",
            "sim/flightmodel/position/y_agl",
            "sim/flightmodel/position/phi",
            "sim/flightmodel/position/theta",
            "sim/flightmodel/position/psi",
            "sim/cockpit/switches/gear_handle_status"
        };
        float[] posi = new float[] {37.524F, -122.06899F, 2500, 0, 0, 0, 1};
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.pauseSim(true);
            xpc.sendPOSI(posi);
            //TODO: It seems that these calls are a bit too fast. The dref request often gets stale data, causing the test to fail incorrectly.
            try {Thread.sleep(100);}catch(InterruptedException ex){}
            float[][] result = xpc.requestDREFs(drefs);
            xpc.pauseSim(false);
            if(result.length < posi.length)
            {
                fail();
            }
            assertEquals(posi[0], result[0][0], 1e-4);
            assertEquals(posi[1], result[1][0], 1e-4);
            assertEquals(posi[2], result[2][0], 10);
            assertEquals(posi[3], result[3][0], 1e-4);
            assertEquals(posi[4], result[4][0], 1e-4);
            assertEquals(posi[5], result[5][0], 1e-4);
            assertEquals(posi[6], result[6][0], 1e-4);
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendPOSI_NullCtrl() throws IOException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendPOSI(null);
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendPOSI_LongCtrl() throws IOException
    {
        float[] posi = new float[] {37.524F, -122.06899F, 2500, 0, 0, 0, 1, -998};
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendPOSI(posi);
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendPOSI_NegativeAircraftNum() throws IOException
    {
        float[] posi = new float[] {37.524F, -122.06899F, 2500, 0, 0, 0, 1, -998};
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendPOSI(posi, -1);
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendPOSI_LargeAircraftNum() throws IOException
    {
        float[] posi = new float[] {37.524F, -122.06899F, 2500, 0, 0, 0, 1, -998};
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendPOSI(posi, 300);
        }
    }

    @Test
    public void testSendDATA() throws IOException
    {
        float[][] data = {{25,0.8F,-988,-988,-988,-988,-988,-988,-988}};
        String dref = "sim/flightmodel/engine/ENGN_thro";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendDATA(data);
            float[] result = xpc.requestDREF(dref);
            assertEquals(data[0][1], result[0], 1e-4);
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendData_NullData() throws IOException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendDATA(null);
        }
        fail();
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSendData_InvalidData() throws IOException
    {
        float[][] data = {{25,0.8F,-988,-988,-988,-988,-988,-988,-988,-988}};
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.sendDATA(data);
        }
        fail();
    }

    @Test
    public void testSetCONN() throws IOException
    {
        String dref = "sim/cockpit/switches/gear_handle_status";
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            int p = xpc.getRecvPort();
            xpc.setCONN(49055);
            assertEquals(49055, xpc.getRecvPort());
            float[] result = xpc.requestDREF(dref);
            assertEquals(1, result.length);

            xpc.setCONN(p);
            assertEquals(p, xpc.getRecvPort());
            result = xpc.requestDREF(dref);
            assertEquals(1, result.length);
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSetCONN_NegativePort() throws IOException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.setCONN(-1);
            fail();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSetCONN_LargePort() throws IOException
    {
        try(XPlaneConnect xpc = new XPlaneConnect())
        {
            xpc.setCONN(65536);
            fail();
        }
    }
}