cmake_minimum_required(VERSION 3.10)
project(opencv_demo)

find_package(OpenCV REQUIRED)

add_executable(opencv_demo src/main.cpp)
target_link_libraries(opencv_demo ${OpenCV_LIBS})