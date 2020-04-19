from time import sleep
import xpc

def record(path, interval = 0.1, duration = 60):
    try:
        fd = open(path, "w")
    except:
        print("Unable to open file.")
        return

    count = int(duration / interval)
    if count < 1:
        print("duration is less than a single frame.")
        return

    with xpc.XPlaneConnect("localhost", 49009, 0, 1000) as client:
        print("Recording...")
        for i in range(0, count):
            try:
                posi = client.getPOSI()
                fd.write("{0}, {1}, {2}, {3}, {4}, {5}, {6}\n".format(*posi))
            except:
                print("Error reading position")
                continue
            sleep(interval);
        print("Recording Complete")
    fd.close()

def playback(path, interval):
    try:
        fd = open(path, "r")
    except:
        print("Unable to open file.")
        return

    with xpc.XPlaneConnect("localhost", 49009, 0, 1000) as client: 
        print("Starting Playback...")
        for line in fd:
            try:
                posi = [ float(x) for x in line.split(',') ]
                posi = client.sendPOSI(posi)
            except:
                print("Error sending position")
                continue
            sleep(interval);
        print("Playback Complete")
    fd.close()

def printMenu(title, opts):
    print("\n+---------------------------------------------- +")
    print("|  {0:42}   |\n".format(title))
    print("+---------------------------------------------- +")
    for i in range(0,len(opts)):
        print("| {0:2}. {1:40} |".format(i + 1, opts[i]))
    print("+---------------------------------------------- +")
    return int(input("Please select and option: "))

def ex():
    print("X-Plane Connect Playback Example  [Version 1.2.0]")
    print("(c) 2013-2015 United States Government as represented by the Administrator")
    print("of the National Aeronautics and Space Administration. All Rights Reserved.")

    mainOpts = [ "Record X-Plane", "Playback File", "Exit" ]

    while True:
        opt = printMenu("What would you like to do?", mainOpts)
        if opt == 1:
            path = input("Enter save file path: ")
            interval = float(input("Enter interval between frames (seconds): "))
            duration = float(input("Enter duration to record for (seconds): "))
            record(path, interval, duration)
        elif opt == 2:
            path = input("Enter save file path: ")
            interval = float(input("Enter interval between frames (seconds): "))
            playback(path, interval)
        elif opt == 3:
            return;
        else:
            print("Unrecognized option.")

if __name__ == "__main__":
    ex()