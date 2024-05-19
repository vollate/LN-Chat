#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <shared_mutex>
#include <sys/types.h>
#include <utility>

#include "storage.hpp"
#include "utils.hpp"

Client::Client(uint64_t user_id, std::string username, std::string ip)
    : user_id(user_id), username(std::move(username)), ip(std::move(ip)), alive(true) {}

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

// Terrible design for huge class, but who cares?
void Storage::cleanup() {
    std::unique_lock client_lock{ m_mutex };
    std::lock_guard list_lock{ m_to_delete.first };
    for(auto& client : m_to_delete.second) {
        dPrint("remote dead client: " << client->user_id << ", name: " << client->username);
        m_clients.erase(client->user_id);
    }
    client_lock.unlock();
    m_to_delete.second.clear();
}

bool Storage::addRoom(const std::string& room_name, const std::string& password) {
    std::lock_guard lock{ m_mutex };
    if(m_chatrooms.find(room_name) != m_chatrooms.end()) {
        return false;
        ;
    }
    m_chatrooms.insert({ room_name, std::make_shared<ChatRoom>(room_name, password) });
    return true;
}

std::optional<std::shared_ptr<ChatRoom>> Storage::getRoom(const std::string& room_name) {
    std::shared_lock lock{ m_mutex };
    auto it = m_chatrooms.find(room_name);
    if(it == m_chatrooms.end()) {
        return std::nullopt;
    }
    return it->second;
}

Storage::~Storage() {
    stopFlush();
}
std::optional<uint64_t> Storage::registerClient(const std::string& username, const std::string& ip) {
    std::lock_guard lock{ m_mutex };
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<uint64_t> uniform_dist(0, UINT64_MAX);
    uint64_t user_id;
    for(int i = 0; i < 200; ++i) {
        user_id = uniform_dist(e1);
        if(m_clients.find(user_id) == m_clients.end()) {
            m_clients.insert({ user_id, std::make_shared<Client>(user_id, username, ip) });
            return user_id;
        }
    }
    return std::nullopt;
}
