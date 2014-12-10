import random
import unittest
import imp

xplaneConnect = imp.load_source('xplaneConnect', '../../Python/src/xplaneConnect.py')

class XPCTests(unittest.TestCase):
    """Tests the functionality of the XPlaneConnect class."""

    def test_init(self):
        xpc = xplaneConnect.XPlaneConnect(49062, "127.0.0.1", 49009)
        self.assertTrue(True)

    def test_close(self):
        xpc = xplaneConnect.XPlaneConnect(49063, "127.0.0.1", 49009)
        xpc.close()
        self.assertIsNone(xpc.socket)
        xpc = xplaneConnect.XPlaneConnect(49063, "127.0.0.1", 49009)
        self.assertTrue(True)

    def test_send_read(self):
        # Init
        test = "\x00\x01\x02\x03\x05"

        # Setup
        sender = xplaneConnect.XPlaneConnect(49064, "127.0.0.1", 49063)
        receiver = xplaneConnect.XPlaneConnect(49063, "127.0.0.1", 49009)

        # Execution
        sender.send_udp(test)
        buf = receiver.read_udp()

        # Cleanup
        sender.close()
        receiver.close()

        # Tests
        for a, b in zip(test, buf):
            self.assertEqual(a, b)

    def test_request_dref(self):
        # Init
        dref_array = []

        # Setup
        sender = xplaneConnect.XPlaneConnect(49064, "127.0.0.1", 49009)
        dref_array.append("sim/cockpit/switches/gear_handle_status")
        dref_array.append("cockpit2/switches/panel_brightness_ratio")

        # Execution
        result = sender.request_dref(dref_array)

        # Cleanup
        sender.close()
        receiver.close()

        # Tests
        self.assertEqual(2, len(result))
        self.assertEqual(1, len(result[0]))
        self.assertEqual(4, len(result[1]))

    def test_send_dref(self):
        # Init
        dref_array = []

        # Setup
        sender = xplaneConnect.XPlaneConnect(49066, "127.0.0.1", 49009)
        receiver = xplaneConnect.XPlaneConnect(49008, "127.0.0.1", 49009)
        dref_array.append("sim/cockpit/switches/gear_handle_status")

        # Execution
        sender.send_dref(dref_array[0])
        result = receiver.request_dref(dref_array)

        # Cleanup
        sender.close()
        receiver.close()

        # Tests
        self.assertEqual(1, len(result))
        self.assertEqual(1, len(result[0]))
        self.assertEqual(0.0, result[0][0])

if __name__ == '__main__':
    unittest.main()