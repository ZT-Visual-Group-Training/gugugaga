cmake_minimum_required(VERSION 3.10)
project(OpenCVDemo)

#防止中文乱码
if(MSVC)
  add_compile_options(/utf-8)
endif()

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

#  查找 OpenCV 包
#    REQUIRED 表示如果找不到就直接报错，而不是继续往下走
find_package(OpenCV REQUIRED)

#  创建可执行程序
add_executable(opencv_app main.cpp)

#  把 OpenCV 的头文件路径告诉编译器
target_include_directories(opencv_app PRIVATE ${OpenCV_INCLUDE_DIRS})

#  链接 OpenCV 的库文件
target_link_libraries(opencv_app PRIVATE ${OpenCV_LIBS})