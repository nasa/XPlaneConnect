import random
import unittest
import imp
import time

xpc = imp.load_source('xpc', '../../Python/src/xpc.py')

class XPCTests(unittest.TestCase):
    """Tests the functionality of the XPlaneConnect class."""

    def test_init(self):
        try:
            client = xpc.XPlaneConnect()
        except:
            self.fail("Default constructor failed.")
            
        try:
            client = xpc.XPlaneConnect("I'm not a real host")
            self.fail("Failed to catch invalid XP host.")
        except ValueError:
            pass
        
        try:
            client = xpc.XPlaneConnect("127.0.0.1", 90001)
            self.fail("Failed to catch invalid XP port.")
        except ValueError:
            pass
        try:
            client = xpc.XPlaneConnect("127.0.0.1", -1)
            self.fail("Failed to catch invalid XP port.")
        except ValueError:
            pass

        try:
            client = xpc.XPlaneConnect("127.0.0.1", 49009, 90001)
            self.fail("Failed to catch invalid local port.")
        except ValueError:
            pass
        try:
            client = xpc.XPlaneConnect("127.0.0.1", 49009, -1)
            self.fail("Failed to catch invalid XP port.")
        except ValueError:
            pass
        
        try:
            client = xpc.XPlaneConnect("127.0.0.1", 49009, 0, -1)
            self.fail("Failed to catch invalid timeout.")
        except ValueError:
            pass

    def test_close(self):
        client = xpc.XPlaneConnect("127.0.0.1", 49009, 49063)
        client.close()
        self.assertIsNone(client.socket)
        client = xpc.XPlaneConnect("127.0.0.1", 49009, 49063)

    def test_send_read(self):
        # Init
        test = "\x00\x01\x02\x03\x05"

        # Setup
        sender = xpc.XPlaneConnect("127.0.0.1", 49063, 49064)
        receiver = xpc.XPlaneConnect("127.0.0.1", 49009, 49063)

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
        client = xpc.XPlaneConnect()
        drefs = ["sim/cockpit/switches/gear_handle_status",\
                 "sim/cockpit2/switches/panel_brightness_ratio"]

        # Execution
        result = client.getDREFs(drefs)

        # Cleanup
        client.close()

        # Tests
        self.assertEqual(2, len(result))
        self.assertEqual(1, len(result[0]))
        self.assertEqual(4, len(result[1]))

    def test_sendDREF(self):
        dref = "sim/cockpit/switches/gear_handle_status"
        value = None
        def do_test():
            # Setup
            client = xpc.XPlaneConnect()

            # Execute
            client.sendDREF(dref, value)
            result = client.getDREF(dref)

            # Cleanup
            client.close()

            # Tests
            self.assertEqual(1, len(result))
            self.assertEqual(value, result[0])

        # Test 1
        value = 1
        do_test()

        # Test 2
        value = 0
        do_test()

    def test_sendDREFs(self):
        drefs = [\
            "sim/cockpit/switches/gear_handle_status",\
            "sim/cockpit/autopilot/altitude"]
        values = None
        def do_test():
            # Setup
            client = xpc.XPlaneConnect()

            # Execute
            client.sendDREFs(drefs, values)
            result = client.getDREFs(drefs)

            # Cleanup
            client.close()

            # Tests
            self.assertEqual(2, len(result))
            self.assertEqual(1, len(result[0]))
            self.assertEqual(values[0], result[0][0])
            self.assertEqual(1, len(result[1]))
            self.assertEqual(values[1], result[1][0])

        # Test 1
        values = [1, 2000]
        do_test()

        # Test 2
        values = [0, 4000]
        do_test()

    def test_sendDATA(self):
        # Setup
        dref = "sim/aircraft/parts/acf_gear_deploy"
        data = [[ 14, 1, 0, -998, -998, -998, -998, -998, -998 ]]
        client = xpc.XPlaneConnect()

        # Execute
        client.sendDATA(data)
        result = client.getDREF(dref)

        # Cleanup
        client.close()

        #Tests
        self.assertEqual(result[0], data[0][1])

    def test_pauseSim(self):
        dref = "sim/operation/override/override_planepath"
        value = None
        expected = None
        def do_test():
            # Setup
            client = xpc.XPlaneConnect()

            # Execute
            client.pauseSim(value)
            result = client.getDREF(dref)
        
            # Cleanup
            client.close()

            # Test
            self.assertAlmostEqual(expected, result[0])

        # Test 1
        value = True
        expected = 1.0
        do_test()

        # Test 2
        value = False
        expected = 0.0
        do_test()

        # Test 3
        value = 1
        expected = 1.0
        do_test()

        # Test 4
        value = 2
        expected = 0.0
        do_test()

    def test_getCTRL(self):
        values = None
        ac = 0
        expected = None
        def do_test():
            with xpc.XPlaneConnect() as client:
                # Execute
                client.sendCTRL(values, ac)
                result = client.getCTRL(ac)

                # Test
                self.assertEqual(len(result), len(expected))
                for a, e in zip(result, expected):
                    self.assertAlmostEqual(a, e, 4)
                    
        values = [0.0, 0.0, 0.0, 0.8, 1.0, 0.5, -1.5]
        expected = values
        ac = 0
        do_test()

        ac = 3
        do_test()

        
    def test_sendCTRL(self):
        # Setup
        drefs = ["sim/cockpit2/controls/yoke_pitch_ratio",\
		         "sim/cockpit2/controls/yoke_roll_ratio",\
		         "sim/cockpit2/controls/yoke_heading_ratio",\
		         "sim/flightmodel/engine/ENGN_thro",\
		         "sim/cockpit/switches/gear_handle_status",\
		         "sim/flightmodel/controls/flaprqst"]
        ctrl = []

        def do_test():            
            client = xpc.XPlaneConnect()

            # Execute
            client.sendCTRL(ctrl)
            result = client.getDREFs(drefs)

            # Cleanup
            client.close()

            # Tests
            self.assertEqual(6, len(result))
            for i in range(6):
                self.assertAlmostEqual(ctrl[i], result[i][0], 4)

        # Test 1
        ctrl = [ -1.0, -1.0, -1.0, 0.0, 1.0, 1.0 ]
        do_test()

        # Test 2
        ctrl = [ 1.0, 1.0, 1.0, 0.0, 1.0, 0.5 ]
        do_test()

        # Test 2
        ctrl = [ 0.0, 0.0, 0.0, 0.8, 1.0, 0.0 ]
        do_test()

    def test_sendCTRL_speedbrake(self):
        # Setup
        dref = "sim/flightmodel/controls/sbrkrqst"
        ctrl = []

        def do_test():            
            client = xpc.XPlaneConnect()

            # Execute
            client.sendCTRL(ctrl)
            result = client.getDREF(dref)

            # Cleanup
            client.close()

            # Tests
            self.assertAlmostEqual(result[0], ctrl[6])

        # Test 1
        ctrl = [-998, -998, -998, -998, -998, -998, -0.5]
        do_test()

        # Test 2
        ctrl[6] = 1.0
        do_test()

        # Test 2
        ctrl[6] = 0.0
        do_test()

    def test_getPOSI(self):
        values = None
        ac = 0
        expected = None
        def do_test():
            with xpc.XPlaneConnect() as client:
                # Execute
                client.pauseSim(True)
                client.sendPOSI(values, ac)
                result = client.getPOSI(ac)
                client.pauseSim(False)

                # Test
                self.assertEqual(len(result), len(expected))
                for a, e in zip(result, expected):
                    self.assertAlmostEqual(a, e, 4)
                    
        values = [ 37.524, -122.06899, 2500, 45, -45, 15, 1 ]
        expected = values
        ac = 0
        do_test()

        ac = 3
        do_test()

    def test_sendPOSI(self):
        # Setup
        drefs = ["sim/flightmodel/position/latitude",\
		         "sim/flightmodel/position/longitude",\
		         "sim/flightmodel/position/elevation",\
		         "sim/flightmodel/position/theta",\
		         "sim/flightmodel/position/phi",\
		         "sim/flightmodel/position/psi",\
		         "sim/cockpit/switches/gear_handle_status"]
        posi = None
        def do_test():
            client = xpc.XPlaneConnect()

            # Execute
            client.pauseSim(True)
            client.sendPOSI(posi)
            result = client.getDREFs(drefs)
            client.pauseSim(False)

            # Cleanup
            client.close()

            # Tests
            self.assertEqual(7, len(result))
            for i in range(7):
                self.assertAlmostEqual(posi[i], result[i][0], 4)

        # Test 1
        posi = [ 37.524, -122.06899, 2500, 5, 7, 11, 1 ]
        do_test()

        # Test 2
        posi = [ 38, -121.0, 2000, -10, 0, 0, 0 ]
        do_test()

    def test_sendTEXT(self):
        # Setup
        client = xpc.XPlaneConnect()
        x = 200
        y = 700
        msg = "Python sendTEXT test message."

        # Execution
        client.sendTEXT(msg, x, y)
        # NOTE: Manually verify that msg appears on the screen in X-Plane

        # Cleanup
        client.close()

    def test_sendView(self):
        # Setup
        dref = "sim/graphics/view/view_type"
        fwd = 1000
        chase = 1017

        #Execution
        with xpc.XPlaneConnect() as client:
            client.sendVIEW(xpc.ViewType.Forwards)
            result = client.getDREF(dref)
            self.assertAlmostEqual(fwd, result[0], 1e-4)
            client.sendVIEW(xpc.ViewType.Chase)
            result = client.getDREF(dref)
            self.assertAlmostEqual(chase, result[0], 1e-4)
             

    def test_sendWYPT(self):
        # Setup
        client = xpc.XPlaneConnect()
        points = [\
		    37.5245, -122.06899, 2500,\
		    37.455397, -122.050037, 2500,\
		    37.469567, -122.051411, 2500,\
		    37.479376, -122.060509, 2300,\
		    37.482237, -122.076130, 2100,\
		    37.474881, -122.087288, 1900,\
		    37.467660, -122.079391, 1700,\
		    37.466298, -122.090549, 1500,\
		    37.362562, -122.039223, 1000,\
		    37.361448, -122.034416, 1000,\
		    37.361994, -122.026348, 1000,\
		    37.365541, -122.022572, 1000,\
		    37.373727, -122.024803, 1000,\
		    37.403869, -122.041283, 50,\
		    37.418544, -122.049222, 6]

        # Execution
        client.sendPOSI([37.5245, -122.06899, 2500])
        client.sendWYPT(3, [])
        client.sendWYPT(1, points)
        # NOTE: Manually verify that points appear on the screen in X-Plane

        # Cleanup
        client.close()

    def test_setCONN(self):
        # Setup
        dref = "sim/cockpit/switches/gear_handle_status";
        client = xpc.XPlaneConnect()

        # Execute
        client.setCONN(49055)
        result = client.getDREF(dref)

        # Cleanup
        client.close()
        
        # Test
        self.assertEqual(1, len(result))



if __name__ == '__main__':
    unittest.main()