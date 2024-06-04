#include "RpcClient.hpp"
#include <iostream>

RpcClient::RpcClient(const std::string& ip, const std::string& port)
    : stub_(LN_Chat::ChatService::NewStub(grpc::CreateChannel(construct_address(ip, port), grpc::InsecureChannelCredentials()))) {
}

RpcClient::~RpcClient() {}

std::string RpcClient::construct_address(const std::string& ip, const std::string& port) {
    return ip + ":" + port;
}

bool RpcClient::RegisterClient(const std::string& name, uint64_t& clientId) {
    LN_Chat::RegisterClientRequest request;
    request.set_name(name);

    LN_Chat::RegisterClientReply reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->RegisterClient(&context, request, &reply);

    if(status.ok() && reply.success()) {
        clientId = reply.clientid();
        return true;
    } else {
        std::cout << "RegisterClient failed: " << reply.message() << std::endl;
        return false;
    }
}

bool RpcClient::PublishRoom(uint64_t clientId, const std::string& name, const std::string& password) {
    LN_Chat::PublishRoomRequest request;
    request.set_clientid(clientId);
    request.set_name(name);
    request.set_password(password);

    LN_Chat::PublishRoomReply reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->PublishRoom(&context, request, &reply);

    if(status.ok() && reply.success()) {
        return true;
    } else {
        std::cout << "PublishRoom failed: " << reply.message() << std::endl;
        return false;
    }
}

bool RpcClient::GetRoomPeers(uint64_t clientId, const std::string& name, const std::string& password,
                             std::vector<PeerInfo>& peers) {
    LN_Chat::GetRoomPeersRequest request;
    request.set_clientid(clientId);
    request.set_name(name);
    request.set_password(password);

    LN_Chat::GetRoomPeersReply reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->GetRoomPeers(&context, request, &reply);

    if(status.ok() && reply.success()) {
        // Clear existing peers and reserve space for new entries
        peers.clear();
        peers.reserve(reply.peers_size());

        // Iterate through each Peer in the reply
        for(const auto& peer : reply.peers()) {
            PeerInfo peerInfo;
            peerInfo.ip = peer.ip();
            peerInfo.name = peer.name();
            peers.push_back(peerInfo);
        }

        return true;
    } else {
        std::cout << "GetRoomPeers failed: " << reply.message() << std::endl;
        return false;
    }
}

bool RpcClient::HeartBeat(uint64_t clientId) {
    LN_Chat::HeartBeatRequest request;
    request.set_clientid(clientId);

    LN_Chat::HeartBeatReply reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->HeartBeat(&context, request, &reply);

    if(status.ok() && reply.success()) {
        return true;
    } else {
        std::cout << "HeartBeat failed" << std::endl;
        return false;
    }
}

bool RpcClient::GetAllRooms(uint64_t clientId, std::vector<std::string>& rooms) {
    LN_Chat::GetAllRoomsRequest request;
    request.set_clientid(clientId);

    LN_Chat::GetAllRoomsReply reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->GetAllRooms(&context, request, &reply);

    if(status.ok() && reply.success()) {
        rooms.clear();
        rooms.reserve(reply.rooms_size());

        for(const auto& room : reply.rooms()) {
            rooms.push_back(room);
        }

        return true;
    } else {
        std::cout << "GetAllRooms failed: " << std::endl;
        return false;
    }
}
