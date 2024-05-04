cmake_minimum_required(VERSION 3.0)  # 指定要求的 CMake 的最低版本

project(Web_Server)  # 指定项目名称

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 引入正确的标准库路径
include_directories(/usr/include/c++/11)  # 注意：请将路径替换为您系统中的正确路径

# 添加您的可执行文件或库
add_executable(YourExecutableName main.cpp)

# 链接所需的库
target_link_libraries(YourExecutableName stdc++)

# 如果需要使用std::memory库，您可以添加以下操作
target_link_libraries(YourExecutableName stdc++fs)