from time import sleep
import xpc

def record(path, interval = 0.1, duration = 60):
    try:
        fd = open(path, "w")
    except:
        print "Unable to open file."
        return
    
    count = duration / interval
    if count < 1:
        print "duration is less than a single frame."
        return

    with xpc.XPlaneConnect() as client: 
        print "Recording..."
        for i in range(0, count):
            try:
                posi = client.getPOSI()
                fd.write("{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}\n".format(posi))
            except:
                continue
            sleep(interval);
        print "Recording Complete"
    fd.close()

def playback(path, interval):
    try:
        fd = open(path, "r")
    except:
        print "Unable to open file."
        return

    with xpc.XPlaneConnect() as client: 
        print "Starting Playback..."
        for line in fd:
            try:
                posi = [ float(x) for x in line.split(',') ]
                posi = client.sendPOSI(posi)
            except:
                continue
            sleep(interval);
        print "Playback Complete"
    fd.close()

def printMenu(title, opts):
    print "\n+---------------------------------------------- +\n"
    print "|  {0:-42}   |\n".format(title)
    print "+---------------------------------------------- +\n"
    for i in range(0,len(opts)):
        print "| {0:2}. {1:-40} |\n".format(i + 1, opts[i])
    print "+---------------------------------------------- +\n"
    return int(raw_input("Please select and option: "))

def ex():
    print "X-Plane Connect Playback Example  [Version 1.2.0]\n"
    print "(c) 2013-2015 United States Government as represented by the Administrator\n"
    print "of the National Aeronautics and Space Administration. All Rights Reserved.\n"

    mainOpts = [ "Record X-Plane", "Playback File", "Exit" ]

    while True:
        opt = printMenu("What would you like to do?", mainOpts)
        if opt == 1:
            path = raw_input("Enter save file path: ")
            interval = int(raw_input("Enter interval between frames (milliseconds): "))
            duration = int(raw_input("Enter duration to record for (seconds): "))
            record(path, interval, duration)
        elif opt == 2:
            path = raw_input("Enter save file path: ")
            interval = int(raw_input("Enter interval between frames (milliseconds): "))
            playback(path, interval)
        elif opt == 3:
            return;
        else:
            print "Unrecognized option."
