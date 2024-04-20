cmake_minimum_required(VERSION 3.16)

project(LN-Chat-Server)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
  add_compile_definitions(LN_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src)
