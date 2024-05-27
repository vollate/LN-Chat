#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <shared_mutex>
#include <string>
#include <thread>
#include <utility>

#include "storage.hpp"
constexpr static int MaxTry = 200;

Client::Client(uint64_t user_id, std::string username, std::string socket_addr)
    : user_id{ user_id }, username{ std::move(username) }, socket_addr{ std::move(socket_addr) }, alive{ true } {}

ChatRoom::ChatRoom(std::string name, std::string password) : room_name{ std::move(name) }, password{ std::move(password) } {}

void Storage::startFlush(const std::chrono::milliseconds& flush_interval) {
    m_flush_running = true;
    m_flush_thread = std::thread([this, flush_interval]() {
        auto lock = getLock(true);
        while(m_flush_running) {
            std::this_thread::sleep_for(flush_interval);
            lock.lock();
            cleanup();
            for(auto& [_, client] : m_clients) {
                if(!client->alive) {
                    m_to_delete.push_back(client);
                } else {
                    client->alive = false;
                }
            }
            lock.unlock();
        }
    });
}

void Storage::stopFlush() {
    m_flush_running = false;
    if(m_flush_thread.has_value()) {
        m_flush_thread->join();
    }
}

// Cleanup client that died in double choice. Free the Room if it's empty after client leaves.
// Terrible design for huge class, but who cares?(no time to optimize)
void Storage::cleanup() {
    std::vector<std::string> rooms_to_delete;
    std::vector<uint64_t> clients_to_delete;

    for(auto& client : m_to_delete) {
        for(auto it = m_chatrooms.begin(); it != m_chatrooms.end();) {
            auto& room = it->second;
            if(room->users.size() == 1 && room->users.front() == client) {
                rooms_to_delete.push_back(it->first);
                ++it;
            } else {
                room->users.remove(client);
                ++it;
            }
        }
        clients_to_delete.push_back(client->user_id);
    }
    for(const auto& room_name : rooms_to_delete) {
        m_chatrooms.erase(room_name);
    }
    for(const auto& user_id : clients_to_delete) {
        m_clients.erase(user_id);
    }
    m_to_delete.clear();
}

auto Storage::addRoom(const std::string& room_name, const std::string& password) -> bool {
    if(m_chatrooms.find(room_name) != m_chatrooms.end()) {
        return false;
    }
    m_chatrooms.insert({ room_name, std::make_shared<ChatRoom>(room_name, password) });
    return true;
}

Storage::~Storage() {
    stopFlush();
}

auto Storage::registerClient(const std::string& username, const std::string& socket_addr) -> std::optional<uint64_t> {
    std::random_device rdv;
    std::default_random_engine eng(rdv());
    std::uniform_int_distribution<uint64_t> uniform_dist(0, UINT64_MAX);
    for(int i = 0; i < MaxTry; ++i) {
        uint64_t user_id = uniform_dist(eng);
        if(m_clients.find(user_id) == m_clients.end()) {
            m_clients.insert({ user_id, std::make_shared<Client>(user_id, username, socket_addr) });
            return user_id;
        }
    }
    return std::nullopt;
}

auto Storage::getChatRoom(const std::string& room_name) -> std::optional<std::shared_ptr<ChatRoom>> {
    if(auto iter = m_chatrooms.find(room_name); iter != m_chatrooms.end()) {
        return iter->second;
    }
    return std::nullopt;
}

auto Storage::getClient(uint64_t user_id) -> std::optional<std::shared_ptr<Client>> {
    if(auto iter = m_clients.find(user_id); iter != m_clients.end()) {
        return iter->second;
    }
    return std::nullopt;
}

auto Storage::activateClient(uint64_t user_id) -> bool {
    if(auto iter = m_clients.find(user_id); iter != m_clients.end()) {
        iter->second->alive = true;
        return true;
    }
    return false;
}

auto Storage::getLock(bool defer) -> std::unique_lock<std::shared_mutex> {
    if(defer) {
        return std::unique_lock{ m_mutex, std::defer_lock };
    }
    return std::unique_lock(m_mutex);
}

auto Storage::getSharedLock(bool defer) -> std::shared_lock<std::shared_mutex> {
    if(defer) {
        return std::shared_lock{ m_mutex, std::defer_lock };
    }
    return std::shared_lock(m_mutex);
}
