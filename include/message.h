#ifndef AB_PROTO_MESSAGE_H
#define AB_PROTO_MESSAGE_H

#include "command_payload.h"
#include "error_payload.h"
#include "handshake_payload.h"

namespace aquabox {
namespace proto {
    enum class MessageType : uint8_t {
        Error = 0,
        InternalError = 1,
        Command = 2,
        Handshake = 3
    };

    struct Message // 256 bytes
    {
        uint8_t serial[SERIAL_LEN]; // 8
        uint8_t token[TOKEN_LEN]; // 8
        uint16_t payloadLength; // 2
        MessageType type; // 1
        union Payload {
            CommandPayload command;
            ErrorPayload error;
            HandshakePayload handshake;
        } payload;
    } __attribute__((packed));
} // namespace proto
} // namespace aquabox

#endif // AB_PROTO_MESSAGE_H