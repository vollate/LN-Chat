#pragma once

#include <grpcpp/completion_queue.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <memory>
#include <thread>
#include <vector>

#include "chat.grpc.pb.h"
#include "chat.pb.h"

class ChatServer final {
public:
    explicit ChatServer(uint16_t port);

    ~ChatServer();

private:
    void handleRpc();

    void handleRpcInThreadPool();

    std::unique_ptr<grpc::Server> m_server;
    LN_Chat::ChatService::AsyncService m_service;
    std::unique_ptr<grpc::ServerCompletionQueue> m_completion_queue;
    std::vector<std::thread> m_workders;
    bool m_shutdown;
};

class CallData final {
public:
    enum RequestType {
        PUBLISH_ROOM, GET_ROOM_PEERS
    };

    CallData(LN_Chat::ChatService::AsyncService *service,
             grpc::ServerCompletionQueue *completion_queue, RequestType type);

    void proceed();

private:
    LN_Chat::ChatService::AsyncService *m_service;
    grpc::ServerCompletionQueue *m_completion_queue;
    grpc::ServerContext m_context;

    LN_Chat::PublishRoomRequest m_publish_room_request;
    LN_Chat::PublishRoomReply m_publish_room_reply;
    grpc::ServerAsyncResponseWriter<LN_Chat::PublishRoomReply>
            m_publish_room_responder;

    LN_Chat::GetRoomPeersRequest m_get_room_peers_request;
    LN_Chat::GetRoomPeersReply m_get_room_peers_reply;
    grpc::ServerAsyncResponseWriter<LN_Chat::GetRoomPeersReply>
            m_get_room_peers_responder;

    enum CallStatus {
        CREATE, PROCESS, FINISH
    };
    CallStatus m_status;
    RequestType m_type;

    void handleCreate();

    void handleProcess();

    void handleFinish();

};
