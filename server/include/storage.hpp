#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>

struct Client {
    Client(uint64_t user_id, std::string username, std::string ip);

    uint64_t user_id;
    std::string username;
    std::string ip;
    std::list<std::string> joined_rooms;
    std::atomic_bool alive;
};

struct ChatRoom {
    ChatRoom(std::string name, std::string password);

    std::string room_name;
    std::string password;
    std::list<std::shared_ptr<Client>> users;
    std::list<std::string> blacklist;
};

class Storage final {
public:
    Storage() = default;
    ~Storage();

    void startFlush(const std::chrono::milliseconds& flush_interval);
    void stopFlush();
    std::optional<uint64_t> registerClient(const std::string& username, const std::string& ip);
    bool addRoom(const std::string& room_name, const std::string& password);
    std::optional<std::shared_ptr<ChatRoom>> getRoom(const std::string& room_name);

private:
    void cleanup();

    std::optional<std::thread> m_flush_thread;
    std::atomic_bool m_flush_running{ false };
    std::shared_mutex m_mutex;
    std::unordered_map<uint64_t, std::shared_ptr<Client>> m_clients;
    std::unordered_map<std::string, std::shared_ptr<ChatRoom>> m_chatrooms;
    std::pair<std::mutex, std::list<std::shared_ptr<Client>>> m_to_delete;
};
