set(SOURCE_FILES "main.cpp" "storage.cpp" "control.cpp")

set(chat_proto_path "${CMAKE_SOURCE_DIR}/../protos/chat.proto")
cmake_path(GET chat_proto_path PARENT_PATH proto_dir)
set(chat_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.cc")
set(chat_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.cc")
set(chat_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.h")
set(chat_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.h")
set(vcpkg_tools "$ENV{VCPKG_ROOT}/installed/x64-linux/tools")
set(protoc_path "${vcpkg_tools}/protobuf/protoc")
set(grpc_plugin_path "${vcpkg_tools}/grpc/grpc_cpp_plugin")

add_custom_command(
  OUTPUT "${chat_proto_srcs}" "${chat_proto_hdrs}" "${chat_grpc_srcs}"
         "${chat_grpc_hdrs}"
  COMMAND
    ${protoc_path} ARGS --grpc_out="${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out="${CMAKE_CURRENT_BINARY_DIR}" -I "${proto_dir}"
    --plugin=protoc-gen-grpc=${grpc_plugin_path} "${chat_proto_path}"
  DEPENDS "${chat_proto_path}")

add_executable(ln_server ${SOURCE_FILES})
add_library(chat_proto ${chat_proto_srcs} ${chat_grpc_srcs} "server.cpp")
target_include_directories(chat_proto PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
                                             "${CMAKE_SOURCE_DIR}/include")

find_package(gRPC CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
target_link_libraries(chat_proto PUBLIC gRPC::gpr gRPC::grpc gRPC::grpc++
                                        gRPC::grpc++_alts)
target_link_libraries(chat_proto PRIVATE absl::strings absl::flags_parse)
target_link_libraries(ln_server PRIVATE chat_proto)
