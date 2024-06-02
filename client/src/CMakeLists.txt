set(SOURCE_FILES
        "main.cpp"
        "Entity/Room.cpp"
        "Entity/Peer.cpp"
        "Managers/ServerManager.cpp"
        "Managers/ClientManager.cpp"
        utils.cpp
        Message.cpp
)

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

add_library(chat_rpc ${chat_proto_srcs} ${chat_grpc_srcs}
        "RpcClient/RpcClient.cpp")

target_include_directories(
        chat_rpc
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>)

find_package(gRPC CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(
        Qt6
        COMPONENTS Core Gui Widgets Network
        REQUIRED)

set(HEADERS ${CMAKE_SOURCE_DIR}/include/ClientManager.hpp)
qt6_wrap_cpp(MOC_SOURCES ${HEADERS})

target_link_libraries(chat_rpc PUBLIC gRPC::gpr gRPC::grpc gRPC::grpc++
        gRPC::grpc++_alts)
target_link_libraries(chat_rpc PRIVATE absl::strings absl::flags_parse)

add_executable(chat_client ${SOURCE_FILES} ${MOC_SOURCES})

target_link_libraries(chat_client PRIVATE chat_rpc)
target_link_libraries(chat_client PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)


