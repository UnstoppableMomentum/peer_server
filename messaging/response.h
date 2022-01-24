/*
* Copyright 2021 <QQQ>
*/

#ifndef MESSAGING_RESPONSE_H_
#define MESSAGING_RESPONSE_H_

#include <string>

#include "messaging/signaling_protocol.h"

std::string makeResponseSuccess();
std::string makeResponseError(EError error);
std::string makeResponseNop();
std::string makeResponseSignIn();
std::string makeResponseSendMessage(const std::string& from, const std::string& message);

#endif  // MESSAGING_RESPONSE_H_
