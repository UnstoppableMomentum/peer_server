/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#include <iostream>
#include <utility>
#include <sstream>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "shared_state.hpp"

#include "logging/logging.h"
#include "websocket_session.hpp"
#include "messaging/response.h"

shared_state::shared_state(std::string doc_root, std::uint32_t max_num_connections)
    : doc_root_(std::move(doc_root))
    , max_num_connections_(max_num_connections) {
    SLNK_LOG_DEBUG() << " max_num_connections:" << max_num_connections_;
}

bool shared_state::join(websocket_session *session) {
    std::lock_guard<std::mutex> lock(mutex_);
    bool res = false;
    if (newConnectionIsAllowed()) {
        sessions_.insert(session);
        res = true;
    }
    return res;
}

void shared_state::leave(websocket_session *session) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(session);
#if defined (DEBUG)
    dump();
#endif
}

std::string shared_state::sendMessage(
    std::string_view from, std::string_view to,
    std::string_view message) {
#if defined(DEBUG)
    dump();
#endif
    std::string response;
    if (!to.empty() && !message.empty()) {
        int res = sendTo(from, to, message);
        if (res > 0) {
            response = makeResponseSuccess();
        } else {
            SLNK_LOG_ERROR() << "Invalid send message request: recipient " << to << " not found";
            response = makeResponseError(EError::recipientNotFound);
        }
    } else {
        std::stringstream ss;
        ss << "Invalid send message request:";

        if (to.empty()) {
           ss << " recipient id is empty";
        }

        if (message.empty()) {
           ss << " message is empty";
        }

        SLNK_LOG_ERROR() << ss.str();
        response = makeResponseError(EError::invalidRequest);
    }
    return response;
}

std::string shared_state::processRequestSignIn(const boost::property_tree::ptree& pt, websocket_session* ws) {
    std::string response = "";
    try {
        const std::string id(pt.get<std::string>("data.id", ""));
        SLNK_LOG_DEBUG() << " id:" << id;
        if (id.empty()) {
            response = makeResponseError(EError::idIsEmpty);
        } else {
            if (exists(id)) {
                response = makeResponseError(EError::idIsAlreadyConnected);
            } else {
                ws->setId(id);
                response = makeResponseSignIn();
            }
        }
    } catch(...) {
        response = makeResponseError(EError::invalidRequest);
    }
#if defined (DEBUG)
    dump();
#endif
    return response;
}

std::string shared_state::processRequestSendMessage(const boost::property_tree::ptree& pt, websocket_session* ws) {
    std::string response = "";
    try {
        const std::string from = ws->getId();
        const std::string to = pt.get<std::string>("data.to", "");
        const std::string message = pt.get<std::string>("data.msg", "");
        response = sendMessage(from, to, message);
    } catch(...) {
        response = makeResponseError(EError::invalidRequest);
    }

    return response;
}

std::string shared_state::handle_message(websocket_session* ws, std::string_view message) {
    SLNK_LOG_DEBUG() << " message:" << message.data();
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
                response = processRequestSignIn(pt, ws);
            break;
            case EMessageId::SIGN_OUT:
                // response = signOut();
            break;
            case EMessageId::SEND_MESSAGE:
                response = processRequestSendMessage(pt, ws);
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
    SLNK_LOG_DEBUG() << " message:" << message;
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

int shared_state::sendTo(
    std::string_view from, std::string_view to, std::string_view message) {

    const std::string messageToSend(makeResponseSendMessage(from, message));

    int res = 0;
    // Put the message in a shared pointer so we can re-use it for each client
    auto const ss = boost::make_shared<std::string const>(
        std::move(messageToSend));

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
                v.emplace_back(p->weak_from_this());
            }
        }
    }

    // For each session in our local list, try to acquire a strong
    // pointer. If successful, then send the message on that session.
    for (auto const &wp : v) {
        SLNK_LOG_DEBUG() << " send to:" << to << " " << message;

        if (auto sp = wp.lock())
            sp->send(ss);
    }
    return res;
}

TSessionsConstItr shared_state::find(std::string_view id) const {
    TSessionsConstItr res = sessions_.end();
    for (TSessionsConstItr it(sessions_.begin()); it != sessions_.end(); ++it) {
        if ((*it)->getId() == id) {
            res = it;
        }
    }
    return res;
}

#if defined (DEBUG)
void shared_state::dump() const {
    SLNK_LOG_DEBUG() << ">>>";
    for (auto p : sessions_) {
        SLNK_LOG_DEBUG() << " id:" << p->getId();
    }
    SLNK_LOG_DEBUG() << "<<<";
}
#endif
