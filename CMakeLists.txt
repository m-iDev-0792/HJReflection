cmake_minimum_required(VERSION 3.20)
project(Reflection)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(HJReflectionTool)
add_subdirectory(HJReflection)

#configure Reflection test project
add_executable(Reflection main.cpp HJTest.cpp ${HJREFLECTION_SRC_FILES})
target_include_directories(Reflection PUBLIC ${HJREFLECTION_INCLUDE_DIR})

set(EXCLUDED_PATH ${HJREFLECTION_EXCLUDED_FILES} ${HJREFLECTIONTOOL_SOURCE_DIR})
#let libclang to find stl lib and other system include files
set(SYSTEM_INCLUDE_PATH /usr/local/include
        /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../include/c++/v1
        /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/12.0.0/include
        /Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/usr/include
        /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include
        /Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/System/Library/Frameworks)
message("our system include path = ${SYSTEM_INCLUDE_PATH}")
message("excluded path = ${EXCLUDED_PATH}")
set(INCLUDE_DIR ${HJREFLECTION_INCLUDE_DIR} ${SYSTEM_INCLUDE_PATH})

# Method 1 to call HJReflectionTool automatically
add_custom_target(HJReflectionToolTarget ALL
        COMMAND HJReflectionTool "${CMAKE_SOURCE_DIR}" "-E${EXCLUDED_PATH}" "-I${INCLUDE_DIR}"
        DEPENDS HJReflectionTool
        WORKING_DIRECTORY "${HJREFLECTIONTOOL_BINARY_DIR}"
        COMMENT "using add_custom_target to run HJReflectionTool to process path ${CMAKE_SOURCE_DIR}"
        VERBATIM
        )
add_dependencies(Reflection HJReflectionToolTarget)


# Method 2 to call HJReflectionTool automatically
#add_custom_command(TARGET Reflection PRE_BUILD
#        COMMAND HJReflectionTool "${CMAKE_SOURCE_DIR}" "-E${EXCLUDED_PATH}" "-I${INCLUDE_DIR}"
#        VERBATIM
#        WORKING_DIRECTORY "${HJREFLECTIONTOOL_BINARY_DIR}"
#        COMMENT "using add_custom_command to run HJReflectionTool to process path ${CMAKE_SOURCE_DIR}"
#        DEPENDS HJReflectionTool)