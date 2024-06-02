add_executable(test_rpc_client test_rpc_client.cpp)
target_link_libraries(test_rpc_client chat_rpc)

find_package(
        Qt6
        COMPONENTS Core Gui Widgets Network
        REQUIRED)
set(HEADERS ${CMAKE_SOURCE_DIR}/include/ClientManager.hpp)
qt6_wrap_cpp(MOC_SOURCES ${HEADERS})
add_executable(test_p2p_server "test_p2p_server.cpp"
        "${MOC_SOURCES}"
        "${CMAKE_SOURCE_DIR}/src/Entity/Room.cpp"
        "${CMAKE_SOURCE_DIR}/src/Entity/Peer.cpp"
        "${CMAKE_SOURCE_DIR}/src/Managers/ServerManager.cpp"
        "${CMAKE_SOURCE_DIR}/src/Managers/ClientManager.cpp"
)

target_link_libraries(test_p2p_server Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
target_link_libraries(test_p2p_server chat_rpc)

add_test(NAME ChatClientTest COMMAND test_rpc_client)
add_test(NAME ChatP2PServerTest COMMAND test_p2p_server)