cmake_minimum_required(VERSION 3.16)
project(hsv_demo LANGUAGES CXX)

find_package(OpenCV REQUIRED)
add_executable(hsv_demo main.cpp)
target_compile_features(hsv_demo PRIVATE cxx_std_17)
target_include_directories(hsv_demo PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(hsv_demo PRIVATE ${OpenCV_LIBS})
configure_file(armor.png armor.png COPYONLY)
