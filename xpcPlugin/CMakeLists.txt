cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(XPlaneConnect)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SDK/CHeaders/XPLM)
include_directories(../C/src)

add_definitions(-DXPLM200 -DLIN=1)

SET(XPLM_LINKER_FLAGS "-shared -rdynamic -nodefaultlibs -undefined_warning")
SET(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${XPLM_LINKER_FLAGS}")

add_library(lin SHARED XPCPlugin.cpp
	xpcPluginTools.cpp
	xpcDrawing.cpp
	../C/src/xplaneConnect.c)

install(TARGETS lin DESTINATION XPlaneConnect/64 RENAME lin.xpl)