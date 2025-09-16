cmake_minimum_required(VERSION 3.0)
project(main)

#设置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 设置头文件搜索路径
# include_directories()
# 设置库文件搜索路径
# link_directories()

# 设置需要编译的源文件列表
set(SRC_LIST ./muduo_server.cpp)

# 设置可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 把.指定路径下的所有.cpp文件都添加到SRC_LIST变量中
# aux_source_directory(. SRC_LIST)

# 表示编译生成可执行文件main，SRC_LIST表示需要编译的源文件列表
add_executable(sever ${SRC_LIST})
# 表示server可执行文件需要链接的库文件
target_link_libraries(sever muduo_net muduo_base pthread)