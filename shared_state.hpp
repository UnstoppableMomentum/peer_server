/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#ifndef SHARED_STATE_HPP_
#define SHARED_STATE_HPP_

#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_set>

#include <boost/smart_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

// Forward declaration
class websocket_session;

using TSessions = std::unordered_set<websocket_session*>;
using TSessionsConstItr = TSessions::const_iterator;

// Represents the shared server state
class shared_state {
    std::string const doc_root_;
    const std::uint32_t max_num_connections_;

    // This mutex synchronizes all access to sessions_
    std::mutex mutex_;

    // Keep a list of all the connected clients
    TSessions sessions_;

    std::string processRequestSignIn(const boost::property_tree::ptree& pt, websocket_session* ws);
    std::string processRequestSendMessage(const boost::property_tree::ptree& pt, websocket_session* ws);

 public:
    explicit shared_state(std::string doc_root, std::uint32_t max_num_connections);

    std::string_view doc_root() const noexcept { return std::string_view(doc_root_); }

    bool join(websocket_session* session);
    void leave(websocket_session* session);

    std::string handle_message(websocket_session* ws, std::string_view message);
    std::string sendMessage(std::string_view from, std::string_view to, std::string_view message);

    void send(std::string message);
    int sendTo(std::string_view from, std::string_view to, std::string_view message);

    TSessionsConstItr begin() const { return sessions_.begin(); }
    TSessionsConstItr end() const { return sessions_.end(); }
    TSessionsConstItr find(std::string_view id) const;
    bool exists(std::string_view id) const { return find(id) != end(); }
    bool newConnectionIsAllowed() const { return (sessions_.size() + 1) <= max_num_connections_; }

#if defined (DEBUG)

 private:
    void dump() const;

#endif
};

#endif  // SHARED_STATE_HPP_

