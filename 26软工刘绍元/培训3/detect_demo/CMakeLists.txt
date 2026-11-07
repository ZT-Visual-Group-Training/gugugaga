cmake_minimum_required(VERSION 3.16)
project(detect_demo LANGUAGES CXX)

find_package(OpenCV REQUIRED)
add_executable(detect_demo main.cpp)
target_compile_features(detect_demo PRIVATE cxx_std_17)
target_include_directories(detect_demo PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(detect_demo PRIVATE ${OpenCV_LIBS})
configure_file(test_clip.mp4 test_clip.mp4 COPYONLY)
