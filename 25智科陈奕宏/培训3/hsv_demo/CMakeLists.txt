cmake_minimum_required(VERSION 3.10)

project(hsv_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 不要写 find_package(OpenCV 4 REQUIRED)，环境是 5.x 时会找不到
find_package(OpenCV REQUIRED)

if(MSVC)
  add_compile_options(/utf-8)
endif()

add_executable(hsv_demo
        main.cpp
        ../common/red_mask.cpp)

target_include_directories(hsv_demo PRIVATE ${OpenCV_INCLUDE_DIRS} ../common)
target_link_libraries(hsv_demo PRIVATE ${OpenCV_LIBS})

# 保留 assets 目录结构，从源码目录或构建目录运行都能找到默认图片。
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/armor.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)