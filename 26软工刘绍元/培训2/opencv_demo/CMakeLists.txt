cmake_minimum_required(VERSION 3.16)
project(OpenCVDemo LANGUAGES CXX)

find_package(OpenCV 4 REQUIRED COMPONENTS core imgcodecs highgui)

add_executable(opencv_demo main.cpp)
target_compile_features(opencv_demo PRIVATE cxx_std_17)
set_target_properties(opencv_demo PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(opencv_demo PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(opencv_demo PRIVATE ${OpenCV_LIBS})
