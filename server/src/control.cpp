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
        // if( std::find(room->blacklist) )
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
