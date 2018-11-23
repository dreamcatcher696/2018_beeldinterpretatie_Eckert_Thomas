cmake_minimum_required(VERSION 3.12)
project(sessie4)

set(CMAKE_CXX_STANDARD 14)

add_executable(sessie4 main.cpp)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )