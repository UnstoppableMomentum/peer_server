/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "./response.h"

std::string getErrorString(EError error) {
    // TODO(qqq) enum to string
    static const char * kErrorMessages[] = {
        "Invalid message",
        "Internal error",
        "Recipient not Found",
        "User id is empty",
        "User is connected already",
        "The maximum number of connections is reached"
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
    root.put("data.cmd", 1);

    std::stringstream ss;
    boost::property_tree::write_json(ss, root);
    return ss.str();
}

std::string makeResponseSendMessage(std::string_view from, std::string_view message) {
    boost::property_tree::ptree root;

    root.put("from", from);
    root.put("msg", message);

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
