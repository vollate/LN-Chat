cmake_minimum_required(VERSION 3.0.0)
project(client VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
# find_package(gRPC CONFIG REQUIRED)

include_directories(include)
add_executable(client main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
# target_link_libraries(${PROJECT_NAME} PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts)

