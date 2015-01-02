cmake_minimum_required(VERSION 2.8.4)

add_executable(main main.cpp)

include_directories(/usr/local/include/xplaneConnect)

find_library(XPLANECONNECT_LIB NAMES xplaneconnect PATHS "/usr/local/lib")

message(STATUS "Library path XPLANECONNECT_LIB is " ${XPLANECONNECT_LIB})

target_link_libraries(main ${XPLANECONNECT_LIB})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
