cmake_minimum_required(VERSION 3.10)
project(opencv_demo)

find_package(OpenCV REQUIRED)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/test.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(opencv_demo src/main.cpp)
target_link_libraries(opencv_demo ${OpenCV_LIBS})