#pragma once
#include <grpcpp/completion_queue.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <memory>

#include "chat.grpc.pb.h"
#include "chat.pb.h"

class ChatServer final {

public:
  explicit ChatServer(uint16_t prot);
  ~ChatServer();

private:
  void handleRpcs();

  std::unique_ptr<grpc::Server> m_server;
  LN_Chat::ChatService::AsyncService m_service;
  std::unique_ptr<grpc::ServerCompletionQueue> m_completion_queue;
};

class CallData {
public:
  CallData(LN_Chat::ChatService::AsyncService *service,
           grpc::ServerCompletionQueue *completion_queue);
  void proceed();

private:
  LN_Chat::ChatService::AsyncService *m_service;
  grpc::ServerCompletionQueue *m_completion_queue;
  grpc::ServerContext m_context;
  LN_Chat::PublishRoomRequest m_request;
  LN_Chat::PublishRoomReply m_reply;
  grpc::ServerAsyncResponseWriter<LN_Chat::PublishRoomReply> m_responder;
  enum CallStatus { CREATE, PROCESS, FINISH };
  CallStatus m_status;
};
