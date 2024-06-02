add_executable(test_rpc_client test_rpc_client.cpp)
target_link_libraries(test_rpc_client chat_rpc)

find_package(
        Qt5
        COMPONENTS Core Gui Widgets Network
        REQUIRED)
set(HEADERS ${CMAKE_SOURCE_DIR}/include/ClientManager.hpp)
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
target_link_libraries(chat_rpc PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

set(client_sources
        "${MOC_SOURCES}"
        "${CMAKE_SOURCE_DIR}/src/Entity/Room.cpp"
        "${CMAKE_SOURCE_DIR}/src/Entity/Peer.cpp"
        "${CMAKE_SOURCE_DIR}/src/Managers/ServerManager.cpp"
        "${CMAKE_SOURCE_DIR}/src/Managers/ClientManager.cpp"
        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
        "${CMAKE_SOURCE_DIR}/src/Entity/Message.cpp")

add_executable(test_p2p_server test_p2p_server.cpp ${client_sources})
target_link_libraries(test_p2p_server chat_rpc )

add_executable(test_ip_parse test_peers_ip_parse.cpp ${client_sources})
target_link_libraries(test_ip_parse Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
target_link_libraries(test_ip_parse chat_rpc )


add_test(NAME ChatClientTest COMMAND test_rpc_client)
add_test(NAME ChatP2PServerTest COMMAND test_p2p_server)
add_test(NAME ChatIPParseTest COMMAND test_ip_parse)
