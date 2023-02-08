@echo off
mkdir build
javac -d build\ discovery\*.java *.java
cd build
jar cfe XPlaneConnect.jar gob.nasa.xpc.XPlaneConnect gov\nasa\xpc\*.class gov\nasa\xpc\discovery\*.class