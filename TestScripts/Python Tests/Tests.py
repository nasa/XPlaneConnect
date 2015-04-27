import random
import unittest
import imp

xplaneConnect = imp.load_source('xplaneConnect', '../../Python/src/xplaneConnect.py')

class XPCTests(unittest.TestCase):
    """Tests the functionality of the XPlaneConnect class."""

    def test_init(self):
        xpc = xplaneConnect.XPlaneConnect()
        self.assertTrue(True)

    def test_close(self):
        xpc = xplaneConnect.XPlaneConnect("127.0.0.1", 49009, 49063)
        xpc.close()
        self.assertIsNone(xpc.socket)
        xpc = xplaneConnect.XPlaneConnect("127.0.0.1", 49009, 49063)
        self.assertTrue(True)

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

    def test_request_dref(self):
        # Setup
        xpc = xplaneConnect.XPlaneConnect()
        drefs = ["sim/cockpit/switches/gear_handle_status", "cockpit2/switches/panel_brightness_ratio"]

        # Execution
        result = xpc.getDREFs(drefs)

        # Cleanup
        xpc.close()

        # Tests
        self.assertEqual(2, len(result))
        self.assertEqual(1, len(result[0]))
        self.assertEqual(4, len(result[1]))

    def test_send_dref(self):
        # Setup
        xpc = xplaneConnect.XPlaneConnect()
        dref = "sim/cockpit/switches/gear_handle_status"

        # Execution
        sender.sendDREF(dref_array)
        result = receiver.getDREF(dref_array)

        # Cleanup
        xpc.close();

        # Tests
        self.assertEqual(1, len(result))
        self.assertEqual(1, len(result[0]))
        self.assertEqual(0.0, result[0][0])

if __name__ == '__main__':
    unittest.main()