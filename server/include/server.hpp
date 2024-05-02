#pragma once
#include <grpcpp/completion_queue.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <memory>

#include "chat.grpc.pb.h"
#include "chat.pb.h"

class ChatServer final : public LN_Chat::ChatService::Service {

public:
  ChatServer() = default;
  ~ChatServer();
  grpc::Status PublishRoom(grpc::ServerContext *context,
                           const LN_Chat::PublishRoomRequest *request,
                           LN_Chat::PublishRoomResponse *response) override {
    return grpc::Status::OK;
  }

private:
  std::unique_ptr<grpc::Server> m_server;
  LN_Chat::ChatService::AsyncService m_service;
  std::unique_ptr<grpc::ServerCompletionQueue> m_completion_queue;
};
