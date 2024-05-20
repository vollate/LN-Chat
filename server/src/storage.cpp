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
#define MAX_TRY 200

Client::Client(uint64_t user_id, std::string username, std::string socket_addr)
    : user_id(user_id), username(std::move(username)), socket_addr(std::move(socket_addr)), alive(true) {}

ChatRoom::ChatRoom(std::string name, std::string password) : room_name(std::move(name)), password(std::move(password)) {}

void Storage::startFlush(const std::chrono::milliseconds& flush_interval) {
    m_flush_running = true;
    m_flush_thread = std::thread([this, flush_interval]() {
        while(m_flush_running) {
            std::this_thread::sleep_for(flush_interval);
            cleanup();
            std::shared_lock s_lock{ m_mutex };
            std::lock_guard lock{ m_to_delete.first };
            for(auto& [_, client] : m_clients) {
                if(!client->alive) {
                    m_to_delete.second.push_back(client);
                } else {
                    client->alive = false;
                }
            }
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
// Terrible design for huge class, but who cares?
void Storage::cleanup() {
    std::unique_lock client_lock{ m_mutex };
    std::lock_guard list_lock{ m_to_delete.first };
    for(auto& client : m_to_delete.second) {
        for(auto& [_, room] : m_chatrooms) {
            if(room->users.size() == 1) {
                m_chatrooms.erase(room->room_name);
            } else {
                room->users.remove(client);
            }
        }
        m_clients.erase(client->user_id);
    }
    client_lock.unlock();
    m_to_delete.second.clear();
}

auto Storage::addRoom(const std::string& room_name, const std::string& password) -> bool {
    std::lock_guard lock{ m_mutex };
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
    std::lock_guard lock{ m_mutex };
    std::random_device rdv;
    std::default_random_engine eng(rdv());
    std::uniform_int_distribution<uint64_t> uniform_dist(0, UINT64_MAX);
    for(int i = 0; i < MAX_TRY; ++i) {
        uint64_t user_id = uniform_dist(eng);
        if(m_clients.find(user_id) == m_clients.end()) {
            m_clients.insert({ user_id, std::make_shared<Client>(user_id, username, socket_addr) });
            return user_id;
        }
    }
    return std::nullopt;
}

auto Storage::getChatRoom(const std::string& room_name)
    -> std::optional<std::pair<std::unique_lock<std::shared_mutex>, std::shared_ptr<ChatRoom>>> {
    std::unique_lock lock{ m_mutex };
    if(auto iter = m_chatrooms.find(room_name); iter != m_chatrooms.end()) {
        return std::make_pair(std::move(lock), iter->second);
    }
    return std::nullopt;
}

auto Storage::getClient(uint64_t user_id) -> std::optional<std::shared_ptr<Client>> {
    std::shared_lock lock{ m_mutex };
    if(auto iter = m_clients.find(user_id); iter != m_clients.end()) {
        return iter->second;
    }
    return std::nullopt;
}

auto Storage::activateClient(uint64_t user_id) -> bool {
    std::lock_guard lock{ m_mutex };
    if(auto iter = m_clients.find(user_id); iter != m_clients.end()) {
        iter->second->alive = true;
        return true;
    }
    return false;
}
