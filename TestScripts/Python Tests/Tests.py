import random
import unittest
import imp

xplaneConnect = imp.load_source('xplaneConnect', '../../Python/src/xplaneConnect.py')

class XPCTests(unittest.TestCase):
    """Tests the functionality of the XPlaneConnect class."""

    def test_init(self):
        try:
            xpc = xplaneConnect.XPlaneConnect()
        except:
            self.fail("Default constructor failed.")
            
        try:
            xpc = xplaneConnect.XPlaneConnect("I'm not a real host")
            self.fail("Failed to catch invalid XP host.")
        except ValueError:
            pass
        
        try:
            xpc = xplaneConnect.XPlaneConnect("127.0.0.1", 90001)
            self.fail("Failed to catch invalid XP port.")
        except ValueError:
            pass
        try:
            xpc = xplaneConnect.XPlaneConnect("127.0.0.1", -1)
            self.fail("Failed to catch invalid XP port.")
        except ValueError:
            pass

        try:
            xpc = xplaneConnect.XPlaneConnect("127.0.0.1", 49009, 90001)
            self.fail("Failed to catch invalid local port.")
        except ValueError:
            pass
        try:
            xpc = xplaneConnect.XPlaneConnect("127.0.0.1", 49009, -1)
            self.fail("Failed to catch invalid XP port.")
        except ValueError:
            pass
        
        try:
            xpc = xplaneConnect.XPlaneConnect("127.0.0.1", 49009, 0, -1)
            self.fail("Failed to catch invalid timeout.")
        except ValueError:
            pass

    def test_close(self):
        xpc = xplaneConnect.XPlaneConnect("127.0.0.1", 49009, 49063)
        xpc.close()
        self.assertIsNone(xpc.socket)
        xpc = xplaneConnect.XPlaneConnect("127.0.0.1", 49009, 49063)

    def test_send_read(self):
        # Init
        test = "\x00\x01\x02\x03\x05"

        # Setup
        sender = xplaneConnect.XPlaneConnect("127.0.0.1", 49063, 49064)
        receiver = xplaneConnect.XPlaneConnect("127.0.0.1", 49009, 49063)

        # Execution
        sender.sendUDP(test)
        buf = receiver.readUDP()

        # Cleanup
        sender.close()
        receiver.close()

        # Tests
        for a, b in zip(test, buf):
            self.assertEqual(a, b)

    def test_getDREFs(self):
        # Setup
        xpc = xplaneConnect.XPlaneConnect()
        drefs = ["sim/cockpit/switches/gear_handle_status",\
                 "sim/cockpit2/switches/panel_brightness_ratio"]

        # Execution
        result = xpc.getDREFs(drefs)

        # Cleanup
        xpc.close()

        # Tests
        self.assertEqual(2, len(result))
        self.assertEqual(1, len(result[0]))
        self.assertEqual(4, len(result[1]))

    def test_sendDREF(self):
        # Setup
        xpc = xplaneConnect.XPlaneConnect()
        dref = "sim/cockpit/switches/gear_handle_status"

        # Execution
        xpc.sendDREF(dref, 0.0)
        result = xpc.getDREF(dref)

        # Cleanup
        xpc.close()

        # Tests
        self.assertEqual(1, len(result))
        self.assertEqual(0.0, result[0])

    def test_pauseSim(self):
        # Setup
        xpc = xplaneConnect.XPlaneConnect()
        dref = "sim/operation/override/override_planepath"

        # Execution 1
        xpc.pauseSim(True)
        result = xpc.getDREF(dref)
        
        # Cleanup 1
        xpc.close()

        # Test 1
        self.assertAlmostEqual(1.0, result[0])

        # Execution 2
        xpc = xplaneConnect.XPlaneConnect()
        xpc.pauseSim(False)
        result = xpc.getDREF(dref)

        # Cleanup 2
        xpc.close()

        # Tests
        self.assertEqual(0.0, result[0])

if __name__ == '__main__':
    unittest.main()