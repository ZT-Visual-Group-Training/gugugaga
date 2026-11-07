cmake_minimum_required(VERSION 3.10)

project(detect_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

if(MSVC)
  add_compile_options(/utf-8)
endif()

add_executable(detect_demo
        main.cpp
        src/armor.cpp
        src/detector.cpp
        ../common/red_mask.cpp)

target_include_directories(detect_demo PRIVATE ${OpenCV_INCLUDE_DIRS} src ../common)
target_link_libraries(detect_demo PRIVATE ${OpenCV_LIBS})

# 保留 assets 目录结构，从源码目录或构建目录运行都能找到默认视频。
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/train3.mp4 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

include(CTest)
if(BUILD_TESTING)
  add_executable(criteria_test
          tests/criteria_test.cpp
          src/armor.cpp
          src/detector.cpp
          ../common/red_mask.cpp)
  target_include_directories(criteria_test PRIVATE ${OpenCV_INCLUDE_DIRS} src ../common)
  target_link_libraries(criteria_test PRIVATE ${OpenCV_LIBS})
  add_test(NAME criteria_test COMMAND criteria_test)
endif()