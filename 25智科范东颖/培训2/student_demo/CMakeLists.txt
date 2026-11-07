cmake_minimum_required(VERSION 3.10)
project(StudentDemo)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(student STATIC tools/student.cpp)
target_include_directories(student PUBLIC ${PROJECT_SOURCE_DIR}/includes)
add_executable(student_app src/main.cpp)
target_link_libraries(student_app PRIVATE student)