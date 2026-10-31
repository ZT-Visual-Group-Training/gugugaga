cmake_minimum_required(VERSION 3.16)
project(StudentDemo LANGUAGES CXX)

add_library(student STATIC tools/student.cpp)
target_include_directories(student PUBLIC includes)
target_compile_features(student PUBLIC cxx_std_17)
set_target_properties(student PROPERTIES CXX_EXTENSIONS OFF)

add_executable(student_demo src/main.cpp)
target_link_libraries(student_demo PRIVATE student)
