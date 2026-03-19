cmake_minimum_required(VERSION 3.14)
project(mini_s3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch cpp-httplib (header-only)
include(FetchContent)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.18.1
)
FetchContent_MakeAvailable(httplib)

add_executable(mini_s3
    src/main.cpp
    src/bucket_manager.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_include_directories(mini_s3 PRIVATE include)
target_link_libraries(mini_s3 PRIVATE httplib::httplib)
