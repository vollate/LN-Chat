set(SOURCE_FILES "main.cpp")

set(chat_proto_path "${CMAKE_SOURCE_DIR}/../protos/chat.proto")
cmake_path(GET chat_proto_path PARENT_PATH proto_dir)
set(chat_proto_srcs "${CMAKE_SOURCE_DIR}/src/chat.pb.cc")
set(chat_grpc_srcs "${CMAKE_SOURCE_DIR}/src/chat.grpc.pb.cc")
set(chat_proto_hdrs "${CMAKE_SOURCE_DIR}/include/chat.pb.h")
set(chat_grpc_hdrs "${CMAKE_SOURCE_DIR}/include/chat.grpc.pb.h")
set(vcpkg_tools "/home/lambillda/SDKs/vcpkg/installed/x64-linux/tools")
set(protoc_path "${vcpkg_tools}/protobuf/protoc")
set(grpc_plugin_path "${vcpkg_tools}/grpc/grpc_cpp_plugin")

add_custom_command(
  OUTPUT "${chat_proto_srcs}" "${chat_proto_hdrs}" "${chat_grpc_srcs}"
         "${chat_grpc_hdrs}"
  COMMAND
    ${protoc_path} ARGS --grpc_out="${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out="${CMAKE_CURRENT_BINARY_DIR}" -I "${proto_dir}"
    --plugin=protoc-gen-grpc=${grpc_plugin_path} "${chat_proto_path}" && mv
    "${CMAKE_CURRENT_BINARY_DIR}/*.pb.h" "${CMAKE_SOURCE_DIR}/include" && mv
    "${CMAKE_CURRENT_BINARY_DIR}/*.pb.cc" "${CMAKE_SOURCE_DIR}/src"
  DEPENDS "${chat_proto_path}")

add_executable(ln_server ${SOURCE_FILES})
add_library(chat_proto ${chat_proto_srcs} ${chat_grpc_srcs} "server.cpp")
target_include_directories(chat_proto PUBLIC "${CMAKE_SOURCE_DIR}/include")

find_package(gRPC CONFIG REQUIRED)
target_link_libraries(chat_proto PUBLIC gRPC::gpr gRPC::grpc gRPC::grpc++
                                        gRPC::grpc++_alts)

target_link_libraries(ln_server PRIVATE chat_proto)
