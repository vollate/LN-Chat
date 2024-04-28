cmake_minimum_required(VERSION 3.16)

project(LN-Chat-Server)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(LN_DEBUG)
  set(CMAKE_CXX_FLAGS "-g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
endif()

set(HEADER_PATH "${CMAKE_SOURCE_DIR}/include")
set(chat_proto_path "${CMAKE_SOURCE_DIR}/../protos/chat.proto")
cmake_path(GET chat_proto_path PARENT_PATH proto_dir)
set(chat_proto_srcs "${CMAKE_SOURCE_DIR}/src/chat.pb.cc")
set(chat_grpc_srcs "${CMAKE_SOURCE_DIR}/src/chat.grpc.pb.cc")
set(chat_proto_hdrs "${CMAKE_SOURCE_DIR}/include/chat.pb.h")
set(chat_grpc_hdrs "${CMAKE_SOURCE_DIR}/include/chat.grpc.pb.h")

add_custom_command(
  OUTPUT "${chat_proto_srcs}" "${chat_proto_hdrs}" "${chat_grpc_srcs}"
         "${chat_grpc_hdrs}"
  COMMAND
    protoc ARGS --grpc_out="${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out="${CMAKE_CURRENT_BINARY_DIR}" -I "${proto_dir}"
    --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` "${chat_proto_path}" && mv
    "${CMAKE_CURRENT_BINARY_DIR}/*.pb.h" "${CMAKE_SOURCE_DIR}/include" && mv
    "${CMAKE_CURRENT_BINARY_DIR}/*.pb.cc" "${CMAKE_SOURCE_DIR}/src"
  DEPENDS "${chat_proto_path}")

add_subdirectory(src)
