cmake_minimum_required(VERSION 2.8.4)

set(LIBXPLANECONNECT_SRC xplaneConnect.c)

add_library(xplaneconnect_dynamic SHARED ${LIBXPLANECONNECT_SRC})
add_library(xplaneconnect_static  STATIC ${LIBXPLANECONNECT_SRC})

set_target_properties(xplaneconnect_dynamic PROPERTIES OUTPUT_NAME "xplaneconnect")
set_target_properties(xplaneconnect_dynamic PROPERTIES VERSION 1.2 SOVERSION 1)

set_target_properties(xplaneconnect_static  PROPERTIES OUTPUT_NAME "xplaneconnect")

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

install(TARGETS xplaneconnect_dynamic xplaneconnect_static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES xplaneConnect.h DESTINATION include/xplaneConnect)
