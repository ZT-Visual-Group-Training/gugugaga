#指定最低版本
cmake_minimum_required(VERSION 3.10)
#给项目起名
project(StudentDemo)

#用c++标准来写代码
set(CMAKE_CXX_STANDARD 11)

#把tools这个文件打包成lib的静态库
add_library(student_lib STATIC tools/student.cpp)

#lib的使用说明书
target_include_directories(student_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)

#把src编译成一个程序
add_executable(student_app src/main.cpp)

#把lib放到app程序里面去
target_link_libraries(student_app PRIVATE student_lib)