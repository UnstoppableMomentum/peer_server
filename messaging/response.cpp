/*
* Copyright 2021 <QQQ>
*/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "messaging/response.h"

std::string getErrorString(EError error) {
    // TODO enum to string
    static const char * kErrorMessages[] = {
        "Invalid message",
        "Internal error"
    };
    return kErrorMessages[static_cast<int>(error)];
}

std::string makeResponseError(EError error) {
    boost::property_tree::ptree root;

    root.put("res", static_cast<int>(EResult::ERROR));
    root.put("error", getErrorString(error));

    std::stringstream ss;
    boost::property_tree::write_json(ss, root);
    return ss.str();
}

std::string makeResponseNop() {
    boost::property_tree::ptree root;

    root.put("res", static_cast<int>(EResult::OK));
    root.put("data", "NOP");

    std::stringstream ss;
    boost::property_tree::write_json(ss, root);
    return ss.str();
}

std::string makeResponseSignIn() {
    boost::property_tree::ptree root;

    root.put("res", static_cast<int>(EResult::OK));
    root.put("data", "signin");

    std::stringstream ss;
    boost::property_tree::write_json(ss, root);
    return ss.str();
}

std::string makeResponseSendMessage() {
    boost::property_tree::ptree root;

    root.put("res", static_cast<int>(EResult::OK));
    root.put("data", "sendmessage");

    std::stringstream ss;
    boost::property_tree::write_json(ss, root);
    return ss.str();
}

std::string makeResponseSuccess() {
    boost::property_tree::ptree root;

    root.put("res", static_cast<int>(EResult::OK));

    std::stringstream ss;
    boost::property_tree::write_json(ss, root);
    return ss.str();
}