set(SOURCE_FILES "main.cpp")

set(chat_proto_path "${CMAKE_SOURCE_DIR}/../protos/chat.proto")
cmake_path(GET chat_proto_path PARENT_PATH proto_dir)
set(chat_proto_srcs "${CMAKE_SOURCE_DIR}/src/chat.pb.cc")
set(chat_grpc_srcs "${CMAKE_SOURCE_DIR}/src/chat.grpc.pb.cc")
set(chat_proto_hdrs "${CMAKE_SOURCE_DIR}/include/chat.pb.h")
set(chat_grpc_hdrs "${CMAKE_SOURCE_DIR}/include/chat.grpc.pb.h")
set(protoc_path "/home/lambillda/SDKs/vcpkg/installed/x64-linux/tools/protobuf")
set(grpc_plugin_path
    "/home/lambillda/SDKs/vcpkg/installed/x64-linux/tools/grpc")
add_custom_command(
  OUTPUT "${chat_proto_srcs}" "${chat_proto_hdrs}" "${chat_grpc_srcs}"
         "${chat_grpc_hdrs}"
  COMMAND
    ${protoc_path}/protoc ARGS --grpc_out="${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out="${CMAKE_CURRENT_BINARY_DIR}" -I "${proto_dir}"
    --plugin=protoc-gen-grpc=${grpc_plugin_path}/grpc_cpp_plugin
    "${chat_proto_path}" && mv "${CMAKE_CURRENT_BINARY_DIR}/*.pb.h"
    "${CMAKE_SOURCE_DIR}/include" && mv "${CMAKE_CURRENT_BINARY_DIR}/*.pb.cc"
    "${CMAKE_SOURCE_DIR}/src" && echo ${protoc_path}
  DEPENDS "${chat_proto_path}")

add_executable(ln_server ${SOURCE_FILES})
add_library(chat_proto ${chat_proto_srcs} ${chat_grpc_srcs})
find_package(gRPC CONFIG REQUIRED)
target_link_libraries(chat_proto PUBLIC gRPC::grpc++ gRPC::grpc++_alts)
target_link_libraries(ln_server PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++
                                        gRPC::grpc++_alts)

target_link_libraries(ln_server PRIVATE chat_proto)
