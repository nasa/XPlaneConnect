cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(XPlaneConnect)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SDK/CHeaders/XPLM)
include_directories(../C/src)

add_definitions(-DXPLM200 -DLIN=1)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_STANDARD 11)

add_library(xpc-linux-64 SHARED XPCPlugin.cpp
	DataManager.cpp
	Drawing.cpp
	Log.cpp
	Message.cpp
	MessageHandlers.cpp
	Timer.cpp
	UDPSocket.cpp)
set_target_properties(xpc-linux-64 PROPERTIES PREFIX "" SUFFIX ".xpl")
set_target_properties(xpc-linux-64 PROPERTIES COMPILE_FLAGS "-m64 -fno-stack-protector" LINK_FLAGS "-shared -rdynamic -nodefaultlibs -undefined_warning -m64 -fno-stack-protector")

add_library(xpc-linux-32 SHARED XPCPlugin.cpp
	DataManager.cpp
	Drawing.cpp
	Log.cpp
	Message.cpp
	MessageHandlers.cpp
	Timer.cpp
	UDPSocket.cpp)
set_target_properties(xpc-linux-32 PROPERTIES PREFIX "" SUFFIX ".xpl")
set_target_properties(xpc-linux-32 PROPERTIES COMPILE_FLAGS "-m32 -fno-stack-protector" LINK_FLAGS "-shared -rdynamic -nodefaultlibs -undefined_warning -m32 -fno-stack-protector")
	
# Switch install targets when uncommenting the 32 bit line above.
install(TARGETS xpc-linux-64 DESTINATION XPlaneConnect/64 RENAME lin.xpl)
install(TARGETS xpc-linux-32 DESTINATION XPlaneConnect/ RENAME lin.xpl)
