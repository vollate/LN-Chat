#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <utility>

#include "storage.hpp"

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

bool Storage::checkOrAddClient(uint64_t user_id, const std::string& username, const std::string& ip) {
    std::lock_guard lock{ m_mutex };
    if(m_clients.find(user_id) != m_clients.end()) {
        return false;
    }
    m_clients.insert({ user_id, std::make_shared<Client>(Client{ user_id, username, ip, {}, true }) });
    return true;
}
