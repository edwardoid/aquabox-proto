#ifndef AB_PROTO_MESSAGE_H
#define AB_PROTO_MESSAGE_H

#include "command_payload.h"
#include "error_payload.h"
#include "handshake_payload.h"

namespace aquabox
{
    namespace proto
    {
        enum class MessageType : uint8_t
        {
            Error = 0,
            InternalError = 1,
            Command = 2,
            Handshake = 3
        };

        struct Message // 256 bytes
        {
            uint8_t serial[SERIAL_LEN];
            uint8_t token[TOKEN_LEN];
            uint16_t payloadLength;
            MessageType type;
            union Payload
            {
                CommandPayload command;
                ErrorPayload error;
                HandshakePayload handshake;
            } payload;
        };
    } // namespace proto
} // namespace aquabox

#endif // AB_PROTO_MESSAGE_H