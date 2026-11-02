cmake_minimum_required(VERSION 3.17)

project(STUDENT_DEMO)

set(CMAKE_CXX_STANDARD 17)



add_library(student_lib STATIC tools/student.cpp)
target_include_directories(student_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)
add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE student_lib)
