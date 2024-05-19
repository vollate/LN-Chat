#include "control.hpp"
#include "server.hpp"
#include "utils.hpp"

void Controller::handlePublishRoom(CallData* call_data) {
    dPrint("PublishRoom: " << call_data->m_publish_room_request.name()
                           << " password: " << call_data->m_publish_room_request.password());
    if(call_data->m_storage->addRoom(call_data->m_publish_room_request.name(), call_data->m_publish_room_request.password())) {
        call_data->m_publish_room_reply.set_success(true);
        call_data->m_publish_room_reply.set_message("Room published successfully");
    } else {
        call_data->m_publish_room_reply.set_success(false);
        call_data->m_publish_room_reply.set_message("Room already exists");
    }
    call_data->m_status = CallData::FINISH;
    call_data->m_publish_room_responder.Finish(call_data->m_publish_room_reply, grpc::Status::OK, call_data);
}

void Controller::handleGetRoomPeers(CallData* call_data) {
    dPrint("GetRoomPeers: " << call_data->m_get_room_peers_request.name()
                            << "client_id=" << call_data->m_get_room_peers_request.clientid());
    if(auto room_opt = call_data->m_storage->getRoom(call_data->m_get_room_peers_request.name())) {
        auto room = room_opt.value();

        for(auto& user : room->users) {
            call_data->m_get_room_peers_reply.add_peers(user->username);
        }
        call_data->m_get_room_peers_reply.set_success(true);
        call_data->m_get_room_peers_reply.set_message("Peers retrieved successfully");
    } else {
        call_data->m_get_room_peers_reply.set_success(false);
        call_data->m_get_room_peers_reply.set_message("Room not found");
    }
    call_data->m_status = CallData::FINISH;
    call_data->m_get_room_peers_responder.Finish(call_data->m_get_room_peers_reply, grpc::Status::OK, call_data);
}

void Controller::handleRegisterClient(CallData* call_data) {
    dPrint("RegisterClient: " << call_data->m_register_client_request.name() << " socket_addr: " << call_data->m_context.peer());
    if(auto client_id =
           call_data->m_storage->registerClient(call_data->m_register_client_request.name(), call_data->m_context.peer())) {
        call_data->m_register_client_reply.set_success(true);
        call_data->m_register_client_reply.set_clientid(client_id.value());
        dPrint("Client registered with id: " << client_id.value());
    } else {
        call_data->m_register_client_reply.set_success(false);
        call_data->m_register_client_reply.set_message("Client full");
    }
    call_data->m_status = CallData::FINISH;
    call_data->m_register_client_responder.Finish(call_data->m_register_client_reply, grpc::Status::OK, call_data);
}

void Controller::handleHeartBeat(CallData* call_data) {
    dPrint("HeartBeat: client_id=" << call_data->m_heart_beat_request.clientid());
    
    call_data->m_status = CallData::FINISH;
    call_data->m_heart_beat_responder.Finish(call_data->m_heart_beat_reply, grpc::Status::OK, call_data);
}
