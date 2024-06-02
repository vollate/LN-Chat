#include "Client.hpp"
#include <iostream>

ChatClient::ChatClient(const std::string &ip, const std::string &port)
        : stub_(LN_Chat::ChatService::NewStub(
        grpc::CreateChannel(construct_address(ip, port), grpc::InsecureChannelCredentials()))) {}

ChatClient::~ChatClient() {
}

std::string ChatClient::construct_address(const std::string &ip, const std::string &port) {
    return ip + ":" + port;
}

bool ChatClient::RegisterClient(const std::string &name, uint64_t &clientId) {
    LN_Chat::RegisterClientRequest request;
    request.set_name(name);

    LN_Chat::RegisterClientReply reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->RegisterClient(&context, request, &reply);

    if (status.ok() && reply.success()) {
        clientId = reply.clientid();
        return true;
    } else {
        std::cout << "RegisterClient failed: " << reply.message() << std::endl;
        return false;
    }
}

bool ChatClient::PublishRoom(uint64_t clientId, const std::string &name, const std::string &password) {
    LN_Chat::PublishRoomRequest request;
    request.set_clientid(clientId);
    request.set_name(name);
    request.set_password(password);

    LN_Chat::PublishRoomReply reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->PublishRoom(&context, request, &reply);

    if (status.ok() && reply.success()) {
        return true;
    } else {
        std::cout << "PublishRoom failed: " << reply.message() << std::endl;
        return false;
    }
}

bool ChatClient::GetRoomPeers(uint64_t clientId, const std::string &name, const std::string &password,
                              std::vector<std::string> &peersIp) {
    LN_Chat::GetRoomPeersRequest request;
    request.set_clientid(clientId);
    request.set_name(name);
    request.set_password(password);

    LN_Chat::GetRoomPeersReply reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->GetRoomPeers(&context, request, &reply);

    if (status.ok() && reply.success()) {
        peersIp.assign(reply.peersip().begin(), reply.peersip().end());
        return true;
    } else {
        std::cout << "GetRoomPeers failed: " << reply.message() << std::endl;
        return false;
    }
}

bool ChatClient::HeartBeat(uint64_t clientId) {
    LN_Chat::HeartBeatRequest request;
    request.set_clientid(clientId);

    LN_Chat::HeartBeatReply reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->HeartBeat(&context, request, &reply);

    if (status.ok() && reply.success()) {
        return true;
    } else {
        std::cout << "HeartBeat failed" << std::endl;
        return false;
    }
}
