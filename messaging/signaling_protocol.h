/*
* Copyright 2021 <QQQ>
*/

#ifndef MESSAGING_SIGNALING_PROTOCOL_H_
#define MESSAGING_SIGNALING_PROTOCOL_H_

enum class EResult {
    OK    = 0,
    ERROR = 1
};

enum class EMessageId {
    UNKNOWN      = -1,
    NOP          =  0,
    SIGN_IN      =  1,
    SIGN_OUT     =  2,
    SEND_MESSAGE =  3,
    MAX          =  4
};

enum class EError {
    invalidRequest = 0,
    internalError  = 1,
};

#endif  // MESSAGING_SIGNALING_PROTOCOL_H_
