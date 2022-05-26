/////////////////////////////////
//                             //
// Copyright (c) 2022 Selenika //
//                             //
/////////////////////////////////

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
    recipientNotFound = 2,
    idIsEmpty = 3,
    idIsAlreadyConnected = 4,
    maxNumConnectionsReached = 5
};

#endif  // MESSAGING_SIGNALING_PROTOCOL_H_
