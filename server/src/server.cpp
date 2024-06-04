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

ChatServer::ChatServer(const std::string& ipv4, uint16_t port, uint32_t num_threads)
    : m_storage{ std::make_shared<Storage>() }, m_shutdown{ false } {
    std::string server_address = ipv4 + ":" + std::to_string(port);
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
    using std::chrono_literals::operator""s;
    m_storage->startFlush(30s);
    for(uint32_t i = 0; i < num_threads; ++i) {
        m_workers.emplace_back(&ChatServer::handleRpcInThreadPool, this);
    }
}

// Shit register way
void ChatServer::handleRpcInThreadPool() {
    new CallData(&m_service, m_completion_queue.get(), CallData::REGISTER_CLIENT, m_storage);
    new CallData(&m_service, m_completion_queue.get(), CallData::HEAR_BEAT, m_storage);
    new CallData(&m_service, m_completion_queue.get(), CallData::PUBLISH_ROOM, m_storage);
    new CallData(&m_service, m_completion_queue.get(), CallData::GET_ROOM_PEERS, m_storage);
    new CallData(&m_service, m_completion_queue.get(), CallData::GET_ALL_ROOMS, m_storage);
    void* tag = nullptr;
    bool success = false;
    while(!m_shutdown) {
        GPR_ASSERT(m_completion_queue->Next(&tag, &success));
        GPR_ASSERT(success);
        static_cast<CallData*>(tag)->proceed();
    }
}

CallData::CallData(ChatService::AsyncService* service, grpc::ServerCompletionQueue* server_cq, RequestType type,
                   std::shared_ptr<Storage> storage)
    : m_service{ service }, m_completion_queue{ server_cq }, m_register_client_responder{ &m_context },
      m_heart_beat_responder{ &m_context }, m_publish_room_responder{ &m_context }, m_get_room_peers_responder{ &m_context },
      m_get_all_rooms_responder{ &m_context }, m_storage{ std::move(storage) }, m_status{ CREATE }, m_type{ type } {
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
        case REGISTER_CLIENT:
            m_service->RequestRegisterClient(&m_context, &m_register_client_request, &m_register_client_responder,
                                             m_completion_queue, m_completion_queue, this);
            break;
        case HEAR_BEAT:
            m_service->RequestHeartBeat(&m_context, &m_heart_beat_request, &m_heart_beat_responder, m_completion_queue,
                                        m_completion_queue, this);
            break;
        case PUBLISH_ROOM:
            m_service->RequestPublishRoom(&m_context, &m_publish_room_request, &m_publish_room_responder, m_completion_queue,
                                          m_completion_queue, this);
            break;
        case GET_ROOM_PEERS:
            m_service->RequestGetRoomPeers(&m_context, &m_get_room_peers_request, &m_get_room_peers_responder, m_completion_queue,
                                           m_completion_queue, this);
            break;
        case GET_ALL_ROOMS:
            m_service->RequestGetAllRooms(&m_context, &m_get_all_rooms_request, &m_get_all_rooms_responder, m_completion_queue,
                                          m_completion_queue, this);
            break;
    }
}

void CallData::handleProcess() {
    new CallData(m_service, m_completion_queue, m_type, m_storage);

    switch(m_type) {
        case REGISTER_CLIENT: {
            Control::handleRegisterClient(this);
            break;
        }
        case HEAR_BEAT: {
            Control::handleHeartBeat(this);
            break;
        }
        case PUBLISH_ROOM: {
            Control::handlePublishRoom(this);
            break;
        }
        case GET_ROOM_PEERS: {
            Control::handleGetRoomPeers(this);
            break;
        }
        case GET_ALL_ROOMS: {
            Control::handleGetAllRooms(this);
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
