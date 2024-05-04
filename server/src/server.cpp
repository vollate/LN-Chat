#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/log/check.h>
#include <absl/strings/str_format.h>
#include <cstdint>
#include <grpc/support/log.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>

#include "server.hpp"

using LN_Chat::ChatService;

ChatServer::ChatServer(uint16_t port) {
  std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&m_service);
  m_completion_queue = builder.AddCompletionQueue();
  m_server = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address << '\n';

  handleRpcs();
}

ChatServer::~ChatServer() { m_completion_queue->Shutdown(); }
void ChatServer::handleRpcs() {
  new CallData(&m_service, m_completion_queue.get());
  void *tag;
  bool ok;
  while (true) {
    GPR_ASSERT(m_completion_queue->Next(&tag, &ok));
    GPR_ASSERT(ok);
    static_cast<CallData *>(tag)->proceed();
  }
}

CallData::CallData(ChatService::AsyncService *service,
                   grpc::ServerCompletionQueue *completion_queue)
    : m_service{service}, m_completion_queue{completion_queue},
      m_responder(&m_context), m_status{CREATE} {
  proceed();
}

void CallData::proceed() {
  if (m_status == CallStatus::CREATE) {
    m_status = CallStatus::PROCESS;
    m_service->RequestPublishRoom(&m_context, &m_request, &m_responder,
                                  m_completion_queue, m_completion_queue, this);
  } else if (m_status == CallStatus::PROCESS) {
    new CallData(m_service, m_completion_queue);
    std::cout << m_request.name() << " password=" << m_request.password()
              << std::endl;
    m_reply.set_success(true);
    m_reply.set_message("OK");
    m_status = CallStatus::FINISH;
    m_responder.Finish(m_reply, grpc::Status::OK, this);
  } else {
    GPR_ASSERT(m_status == CallStatus::FINISH);
    delete this;
  }
}
