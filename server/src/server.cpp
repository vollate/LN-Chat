#include <absl/strings/str_format.h>
#include <cstdint>
#include <grpc/support/log.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <memory>
#include <vector>

#include "server.hpp"
#include "storage.hpp"
#include "utils.hpp"

using LN_Chat::ChatService;
using LN_Chat::GetRoomPeersReply;
using LN_Chat::GetRoomPeersRequest;
using LN_Chat::PublishRoomReply;
using LN_Chat::PublishRoomRequest;
using std::chrono_literals::operator""s;

ChatServer::ChatServer(const std::string& ip, uint16_t port, uint32_t num_threads) : m_shutdown(false) {
    std::string server_address = ip + ":" + std::to_string(port);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&m_service);
    m_completion_queue = builder.AddCompletionQueue();
    m_server = builder.BuildAndStart();
    std::cout << "Server listening on { " << server_address << " }\n";

    handleRpc(num_threads);
}

ChatServer::~ChatServer() {
    m_shutdown = true;
    m_completion_queue->Shutdown();
    for(auto& thread : m_workers) {
        if(thread.joinable()) {
            thread.join();
        }
    }
}

void ChatServer::handleRpc(uint32_t num_threads) {
    m_storage->startFlush(5s);
    for(uint32_t i = 0; i < num_threads; ++i) {
        m_workers.emplace_back(&ChatServer::handleRpcInThreadPool, this);
    }
}

void ChatServer::handleRpcInThreadPool() {
    new CallData(&m_service, m_completion_queue.get(), CallData::PUBLISH_ROOM, m_storage);
    new CallData(&m_service, m_completion_queue.get(), CallData::GET_ROOM_PEERS, m_storage);
    void* tag;
    bool ok;
    while(!m_shutdown) {
        GPR_ASSERT(m_completion_queue->Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<CallData*>(tag)->proceed();
    }
}

CallData::CallData(ChatService::AsyncService* service, grpc::ServerCompletionQueue* cq, RequestType type,
                   std::shared_ptr<Storage> storage)
    : m_service{ service }, m_completion_queue{ cq }, m_publish_room_responder(&m_context),
      m_get_room_peers_responder(&m_context), m_status{ CREATE }, m_type{ type }, m_storage{ std::move(storage) } {
    proceed();
}

void CallData::proceed() {
    switch(m_status) {
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
    switch(m_type) {
        case PUBLISH_ROOM:
            m_service->RequestPublishRoom(&m_context, &m_publish_room_request, &m_publish_room_responder, m_completion_queue,
                                          m_completion_queue, this);
            break;
        case GET_ROOM_PEERS:
            m_service->RequestGetRoomPeers(&m_context, &m_get_room_peers_request, &m_get_room_peers_responder, m_completion_queue,
                                           m_completion_queue, this);
            break;
    }
}

void CallData::handleProcess() {
    new CallData(m_service, m_completion_queue, m_type, m_storage);

    switch(m_type) {
        case PUBLISH_ROOM: {
            Controller::handlePublishRoom(this);
            break;
        }
        case GET_ROOM_PEERS: {
            Controller::handleGetRoomPeers(this);
            break;
        }
        default: {
            dPrint("Unknown request type") break;
        }
    }
}

void CallData::handleFinish() {
    GPR_ASSERT(m_status == FINISH);
    delete this;
}
