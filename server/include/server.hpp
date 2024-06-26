#pragma once

#include <grpcpp/completion_queue.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <memory>
#include <thread>
#include <vector>

#include "chat.grpc.pb.h"
#include "chat.pb.h"
#include "control.hpp"
#include "storage.hpp"

class ChatServer final {
public:
    ChatServer(const std::string& ipv4, uint16_t port, uint32_t num_threads);
    ChatServer(const ChatServer&) = delete;
    auto operator=(const ChatServer&) -> ChatServer& = delete;
    ChatServer(ChatServer&&) = default;
    auto operator=(ChatServer&&) -> ChatServer& = default;
    ~ChatServer();

private:
    void handleRpc(uint32_t num_threads);

    void handleRpcInThreadPool();

    std::shared_ptr<Storage> m_storage;
    std::unique_ptr<grpc::Server> m_server;
    LN_Chat::ChatService::AsyncService m_service;
    std::unique_ptr<grpc::ServerCompletionQueue> m_completion_queue;
    std::vector<std::thread> m_workers;
    bool m_shutdown;
};

class CallData final {
public:
    enum RequestType { REGISTER_CLIENT, PUBLISH_ROOM, GET_ROOM_PEERS, HEAR_BEAT, GET_ALL_ROOMS };

    CallData(LN_Chat::ChatService::AsyncService* service, grpc::ServerCompletionQueue* completion_queue, RequestType type,
             std::shared_ptr<Storage> storage);

    void proceed();

private:
    using Control = Controller;
    friend Control;

    LN_Chat::ChatService::AsyncService* m_service;
    grpc::ServerCompletionQueue* m_completion_queue;
    grpc::ServerContext m_context;

    LN_Chat::RegisterClientRequest m_register_client_request;
    LN_Chat::RegisterClientReply m_register_client_reply;
    grpc::ServerAsyncResponseWriter<LN_Chat::RegisterClientReply> m_register_client_responder;

    LN_Chat::HeartBeatRequest m_heart_beat_request;
    LN_Chat::HeartBeatReply m_heart_beat_reply;
    grpc::ServerAsyncResponseWriter<LN_Chat::HeartBeatReply> m_heart_beat_responder;

    LN_Chat::PublishRoomRequest m_publish_room_request;
    LN_Chat::PublishRoomReply m_publish_room_reply;
    grpc::ServerAsyncResponseWriter<LN_Chat::PublishRoomReply> m_publish_room_responder;

    LN_Chat::GetRoomPeersRequest m_get_room_peers_request;
    LN_Chat::GetRoomPeersReply m_get_room_peers_reply;
    grpc::ServerAsyncResponseWriter<LN_Chat::GetRoomPeersReply> m_get_room_peers_responder;

    LN_Chat::GetAllRoomsRequest m_get_all_rooms_request;
    LN_Chat::GetAllRoomsReply m_get_all_rooms_reply;
    grpc::ServerAsyncResponseWriter<LN_Chat::GetAllRoomsReply> m_get_all_rooms_responder;

    std::shared_ptr<Storage> m_storage;

    enum CallStatus { CREATE, PROCESS, FINISH };

    CallStatus m_status;
    RequestType m_type;

    void handleCreate();

    void handleProcess();

    void handleFinish();
};
