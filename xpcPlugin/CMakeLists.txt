cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(XPlaneConnect)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SDK/CHeaders/XPLM)
include_directories(../C/src)

add_definitions(-DXPLM200 -DLIN=1)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

add_library(xpc64 SHARED XPCPlugin.cpp
	DataManager.cpp
	Drawing.cpp
	Log.cpp
	Message.cpp
	MessageHandlers.cpp
	UDPSocket.cpp)
set_target_properties(xpc64 PROPERTIES PREFIX "" SUFFIX ".xpl")
set_target_properties(xpc64 PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-shared -rdynamic -nodefaultlibs -undefined_warning -m64")

add_library(xpc32 SHARED XPCPlugin.cpp
	DataManager.cpp
	Drawing.cpp
	Log.cpp
	Message.cpp
	MessageHandlers.cpp
	UDPSocket.cpp)
set_target_properties(xpc32 PROPERTIES PREFIX "" SUFFIX ".xpl")
set_target_properties(xpc32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-shared -rdynamic -nodefaultlibs -undefined_warning -m32")
	
# Switch install targets when uncommenting the 32 bit line above.
install(TARGETS xpc64 DESTINATION XPlaneConnect/64 RENAME lin.xpl)
install(TARGETS xpc32 DESTINATION XPlaneConnect/ RENAME lin.xpl)
