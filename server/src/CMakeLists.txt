set(SOURCE_FILES "main.cpp")

add_executable(ln_server ${SOURCE_FILES})
add_library(chat_proto ${chat_proto_srcs} ${chat_grpc_srcs})
target_link_libraries(ln_server PRIVATE chat_proto)
target_include_directories(ln_server PRIVATE ${HEADER_PATH})
target_include_directories(chat_proto PRIVATE ${HEADER_PATH})

find_package(gRPC CONFIG REQUIRED)
target_link_libraries(ln_server PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++
                                        gRPC::grpc++_alts)
