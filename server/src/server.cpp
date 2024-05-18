#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/log/check.h>
#include <absl/strings/str_format.h>
#include <cstdint>
#include <grpc/support/log.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <thread>
#include <vector>

#include "utils.hpp"
#include "server.hpp"

using LN_Chat::ChatService;
using LN_Chat::PublishRoomRequest;
using LN_Chat::PublishRoomReply;
using LN_Chat::GetRoomPeersRequest;
using LN_Chat::GetRoomPeersReply;

ChatServer::ChatServer(uint16_t port) : m_shutdown(false) {
    std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&m_service);
    m_completion_queue = builder.AddCompletionQueue();
    m_server = builder.BuildAndStart();
    std::cout << "Server listening on { " << server_address << " }\n";

    handleRpc();
}

ChatServer::~ChatServer() {
    m_shutdown = true;
    m_completion_queue->Shutdown();
    for (auto &thread: m_workders) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ChatServer::handleRpc() {
    uint32_t num_threads = std::thread::hardware_concurrency();
    for (int i = 0; i < num_threads; ++i) {
        m_workders.emplace_back(&ChatServer::handleRpcInThreadPool, this);
    }
}

void ChatServer::handleRpcInThreadPool() {
    new CallData(&m_service, m_completion_queue.get(), CallData::PUBLISH_ROOM);
    new CallData(&m_service, m_completion_queue.get(), CallData::GET_ROOM_PEERS);
    void *tag;
    bool ok;
    while (!m_shutdown) {
        GPR_ASSERT(m_completion_queue->Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<CallData *>(tag)->proceed();
    }
}

CallData::CallData(ChatService::AsyncService *service, grpc::ServerCompletionQueue *cq, RequestType type)
        : m_service{service}, m_completion_queue{cq},
          m_publish_room_responder(&m_context),
          m_get_room_peers_responder(&m_context),
          m_status{CREATE}, m_type{type} {
    proceed();
}

void CallData::proceed() {
    switch (m_status) {
        case CREATE:
            handleCreate();
            break;
        case PROCESS:
            handleProcess();
            break;
        case FINISH:
            handleFinish();
            break;
    }
}

void CallData::handleCreate() {
    m_status = PROCESS;
    switch (m_type) {
        case PUBLISH_ROOM:
            m_service->RequestPublishRoom(&m_context, &m_publish_room_request, &m_publish_room_responder,
                                          m_completion_queue, m_completion_queue, this);
            break;
        case GET_ROOM_PEERS:
            m_service->RequestGetRoomPeers(&m_context, &m_get_room_peers_request, &m_get_room_peers_responder,
                                           m_completion_queue, m_completion_queue, this);
            break;
    }

}

void CallData::handleProcess() {
    new CallData(m_service, m_completion_queue, m_type);

    switch (m_type) {
        case PUBLISH_ROOM: dPrint("PublishRoom: " << m_publish_room_request.name() << " password: "
                                                  << m_publish_room_request.password());
            m_publish_room_reply.set_success(true);
            m_publish_room_reply.set_message("Room published successfully");
            m_status = FINISH;
            m_publish_room_responder.Finish(m_publish_room_reply, grpc::Status::OK, this);
            break;
        case GET_ROOM_PEERS: dPrint(
                "GetRoomPeers: " << m_get_room_peers_request.name() << " id=" << m_get_room_peers_request.id());
            m_get_room_peers_reply.set_success(true);
            m_get_room_peers_reply.add_peers("peer1");
            m_get_room_peers_reply.add_peers("peer2");
            m_get_room_peers_reply.set_message("Peers retrieved successfully");
            m_status = FINISH;
            m_get_room_peers_responder.Finish(m_get_room_peers_reply, grpc::Status::OK, this);
            break;
        default: dPrint("Unknown request type");
            break;
    }
}

void CallData::handleFinish() {
    GPR_ASSERT(m_status == FINISH);
    delete this;
}
