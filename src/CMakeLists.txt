cmake_minimum_required(VERSION 3.16)
project(src_gm70Rest)
set(PROJECT_NAME gm70rest)
# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
add_compile_definitions(VERSION="0.1.0")


enable_language(C CXX)
set (CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_CXX_STANDARD 17)
add_definitions(-std=c++17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package (Threads)

set(SOURCE_FILES
        cpp-httplib/httplib.h
        json11/json11.hpp json11/json11.cpp
        loguru/loguru.cpp
        inih/INIReader.h
        magic_enum/include/magic_enum.hpp
        serialib/lib/serialib.h serialib/lib/serialib.cpp
        gm70_message.cpp gm70_message.h
        rest_api.cpp rest_api.h
        main.cpp
        gm70_serial_connection.cpp gm70_serial_connection.h msg_queue.cpp msg_queue.h)

file(GLOB SOURCES *.h *.hpp *.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCES})

# MAC COMPILE ON MAC OSX
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES} rt)
endif()

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})


set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gm70rest")
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)