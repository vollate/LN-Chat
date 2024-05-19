#pragma once
class CallData;
struct Controller final {
    Controller() = delete;

    static void handlePublishRoom(CallData* call_data);
    static void handleRegisterClient(CallData* call_data);
    static void handleHeartBeat(CallData* call_data);
    static void handleGetRoomPeers(CallData* call_data);
};
