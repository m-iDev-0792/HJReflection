cmake_minimum_required(VERSION 3.20)
project(HJReflectionTool)
set(CMAKE_CXX_STANDARD 17)

#Configure HJReflectionTool
set(HJREFLECTIONTOOL_BINARY_DIR ${CMAKE_BINARY_DIR} CACHE PATH "where the HJReflectionTool binary file sits" FORCE)
set(HJREFLECTIONTOOL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "where the HJReflectionTool source files sit" FORCE)
set(HJREFLECTIONTOOL_LIBCLANG_PATH "/Applications/Xcode.app/Contents/Frameworks/libclang.dylib" CACHE FILEPATH "The path of the libclang" FORCE)
message("Binary path = ${HJREFLECTIONTOOL_BINARY_DIR}")

add_executable(HJReflectionTool HJReflectionToolEntry.cpp HJReflectionTool.cpp)
target_include_directories(HJReflectionTool PRIVATE ${HJREFLECTIONTOOL_SOURCE_DIR})
target_link_libraries(HJReflectionTool ${HJREFLECTIONTOOL_LIBCLANG_PATH})

