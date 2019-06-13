import sys

import xpc

def monitor():
    with xpc.XPlaneConnect() as client:
        while True:
            posi = client.getPOSI();
            ctrl = client.getCTRL();

            print "Loc: (%4f, %4f, %4f) Aileron:%2f Elevator:%2f Rudder:%2f\n"\
               % (posi[0], posi[1], posi[2], ctrl[1], ctrl[0], ctrl[2])


if __name__ == "__main__":
    monitor()