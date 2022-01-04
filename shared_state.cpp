//
// Copyright (c) 2022 QAZ
//

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "shared_state.hpp"
#include "websocket_session.hpp"
#include "messaging/response.h"

shared_state::
    shared_state(std::string doc_root)
    : doc_root_(std::move(doc_root)) {
}

void shared_state::
    join(websocket_session *session) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(session);
}

void shared_state::
    leave(websocket_session *session) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(session);
}

std::string shared_state::sendMessage(
    const std::string& to, const std::string& message) {
    std::cout << __PRETTY_FUNCTION__
        << " to:" << to
        << " message:" << message << std::endl;

    std::string response;
    if (!to.empty() && !message.empty()) {
        int res = sendTo(message, to);
        if (res > 0) {
            response = makeResponseSuccess();
        }
    } else {
        response = makeResponseError(EError::invalidRequest);
    }
    return response;
}

std::string shared_state::processRequestSignIn(const std::string& message,
    const boost::property_tree::ptree& pt, websocket_session* ws) {
    std::cout << __PRETTY_FUNCTION__ << " message:" << message << std::endl;
    std::string response = "test string";
    try {
        std::string id = pt.get<std::string>("data.id", "");
        std::cout << __PRETTY_FUNCTION__ << " id:" << id << std::endl;

        ws->setId(id);
        response = makeResponseSuccess();
    } catch(...) {
        response = makeResponseError(EError::invalidRequest);
    }

    return response;
}

std::string shared_state::processRequestSendMessage(
    const std::string& message, const boost::property_tree::ptree& pt) {
    std::cout << __PRETTY_FUNCTION__ << " message:" << message << std::endl;
    std::string response = "test string";
    try {
        std::string to = pt.get<std::string>("data.to", "");
        std::string message = pt.get<std::string>("data.msg", "");
        response = sendMessage(to, message);
    } catch(...) {
        response = makeResponseError(EError::invalidRequest);
    }

    return response;
}

std::string shared_state::handle_message(
    websocket_session* ws, std::string message) {
    std::cout << __PRETTY_FUNCTION__ << " message:" << message << std::endl;
    std::string response = "test string";
    try {
        std::stringstream ss;
        ss << message;

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);

        int cmd = pt.get<int>("cmd", -1);

        EMessageId messageId = static_cast<EMessageId> (cmd);
        switch (messageId) {
            case EMessageId::NOP:
                response = makeResponseNop();
            break;
            case EMessageId::SIGN_IN:

                response = processRequestSignIn(message, pt, ws);
            break;
            case EMessageId::SIGN_OUT:
                // response = signOut();
            break;
            case EMessageId::SEND_MESSAGE:
                response = processRequestSendMessage(message, pt);
            break;
            default:
                response = makeResponseError(EError::invalidRequest);
            break;
        }
    } catch(...) {
        response = makeResponseError(EError::invalidRequest);
    }
    return response;
}


// Broadcast a message to all websocket client sessions
void shared_state::send(std::string message) {
    std::cout << __PRETTY_FUNCTION__ << " message:" << message << std::endl;
    // Put the message in a shared pointer so we can re-use it for each client
    auto const ss = boost::make_shared<std::string const>(std::move(message));

    // Make a local list of all the weak pointers representing
    // the sessions, so we can do the actual sending without
    // holding the mutex:
    std::vector<boost::weak_ptr<websocket_session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for (auto p : sessions_)
            v.emplace_back(p->weak_from_this());
    }

    // For each session in our local list, try to acquire a strong
    // pointer. If successful, then send the message on that session.
    for (auto const &wp : v)
        if (auto sp = wp.lock())
            sp->send(ss);
}

int shared_state::sendTo(std::string message, const std::string& to) {
    std::cout << __PRETTY_FUNCTION__ << " message:" << message << std::endl;

    int res = 0;
    // Put the message in a shared pointer so we can re-use it for each client
    auto const ss = boost::make_shared<std::string const>(std::move(message));

    // Make a local list of all the weak pointers representing
    // the sessions, so we can do the actual sending without
    // holding the mutex:
    std::vector<boost::weak_ptr<websocket_session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for (auto p : sessions_) {
            if (p->getId() == to) {
                ++res;
                std::cout << __PRETTY_FUNCTION__ 
                << " res: " << res
                << " FOUND to:" << to << std::endl;
                v.emplace_back(p->weak_from_this());
            }
        }
    }

    // For each session in our local list, try to acquire a strong
    // pointer. If successful, then send the message on that session.
    for (auto const &wp : v) {
    std::cout << __PRETTY_FUNCTION__ << " send to:"
        << to 
        << " " << message 
        << std::endl;

        if (auto sp = wp.lock())
            sp->send(ss);
    }
    return res;
}
