#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>

struct Client {
    Client(uint64_t user_id, std::string username, std::string socket_addr);

    uint64_t user_id;
    std::string username;
    std::string socket_addr;
    std::list<std::string> joined_rooms;
    std::atomic_bool alive;
};

struct ChatRoom {
    ChatRoom(std::string name, std::string password);

    std::string room_name;
    std::string password;
    std::list<std::shared_ptr<Client>> users;
    std::list<std::shared_ptr<Client>> blacklist;
};

class Storage final {
public:
    Storage() = default;
    Storage(const Storage&) = delete;
    auto operator=(const Storage&) -> Storage& = delete;
    Storage(Storage&&) = delete;
    auto operator=(Storage&&) -> Storage& = delete;
    ~Storage();

    void startFlush(const std::chrono::milliseconds& flush_interval);
    void stopFlush();
    auto registerClient(const std::string& username, const std::string& socket_addr) -> std::optional<uint64_t>;
    auto addRoom(const std::string& room_name, const std::string& password) -> bool;
    auto getClient(uint64_t user_id) -> std::optional<std::shared_ptr<Client>>;
    auto getChatRoom(const std::string& room_name) -> std::optional<std::shared_ptr<ChatRoom>>;
    auto activateClient(uint64_t user_id) -> bool;
    auto getLock(bool defer = false) -> std::unique_lock<std::shared_mutex>;
    auto getSharedLock(bool defer = false) -> std::shared_lock<std::shared_mutex>;

    // Provide concurrent access and get for member container
    template <typename C, typename T> auto get(C& container, const T& key) -> std::optional<T> {
        if(auto iter = std::find(container.begin(), container.end(), key); iter != container.end()) {
            return *iter;
        }
        return std::nullopt;
    }

    // Provide concurrent access and add for member container.
    // @return true if the key was added, false if it already exists
    template <typename C, typename T> auto insertOnNotExist(C& container, const T& key) -> bool {
        if(auto iter = std::find(container.begin(), container.end(), key); iter != container.end()) {
            return false;
        }
        container.push_back(key);
        return true;
    }

private:
    void cleanup();

    std::optional<std::thread> m_flush_thread;
    std::atomic_bool m_flush_running{ false };
    std::shared_mutex m_mutex;
    std::unordered_map<uint64_t, std::shared_ptr<Client>> m_clients;
    std::unordered_map<std::string, std::shared_ptr<ChatRoom>> m_chatrooms;
    std::list<std::shared_ptr<Client>> m_to_delete;
};
