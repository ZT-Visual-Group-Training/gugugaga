cmake_minimum_required(VERSION 3.10)
project(opencv_demo)

find_package(OpenCV REQUIRED)

add_executable(opencv_demo main.cpp)
target_include_directories(opencv_demo PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(opencv_demo PRIVATE ${OpenCV_LIBS})
