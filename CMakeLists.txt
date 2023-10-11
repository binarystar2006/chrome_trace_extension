cmake_minimum_required(VERSION 3.10)
project(ChromeTraceSenderReceiver)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 添加编译选项以启用C++多线程支持
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")


# 添加 sender 可执行文件
add_executable(sender sender.cc)

# 添加 receiver 可执行文件
add_library(ctracer SHARED ctracer.cc)


# 添加 cJSON 库的包含目录和链接库
target_include_directories(ctracer PRIVATE /usr/local/include/cjson) # 替换为实际 cjson 库的路径
target_link_libraries(ctracer PRIVATE /usr/local/lib/libcjson.so) # 替换为实际 cjson 库的路径
target_include_directories(ctracer PRIVATE ./) # 替换为实际 cjson 库的路径
target_link_libraries(sender PUBLIC ctracer)


# 安装目标文件
install(TARGETS ctracer DESTINATION lib)
install(TARGETS sender DESTINATION bin)
