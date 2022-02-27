/*
* Copyright 2021 <QQQ>
*/

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


// Represents the shared server state
class shared_state {
    std::string const doc_root_;

    // This mutex synchronizes all access to sessions_
    std::mutex mutex_;

    // Keep a list of all the connected clients
    std::unordered_set<websocket_session*> sessions_;

 public:
    explicit shared_state(std::string doc_root);

    std::string_view doc_root() const noexcept { return std::string_view(doc_root_); }

    void join(websocket_session* session);
    void leave(websocket_session* session);
    std::string processRequestSignIn(const boost::property_tree::ptree& pt, websocket_session* ws);

    std::string handle_message(websocket_session* ws, std::string_view message);
    std::string sendMessage(std::string_view from, std::string_view to, std::string_view message);
    std::string processRequestSendMessage(const boost::property_tree::ptree& pt, websocket_session* ws);

    void send(std::string message);
    int sendTo(std::string_view from, std::string_view to, std::string_view message);
};

#endif  // SHARED_STATE_HPP_

