#ifndef AB_PROTO_MESSAGE_BUILDER_H
#define AB_PROTO_MESSAGE_BUILDER_H

#include "message.h"
#include <string.h>

namespace aquabox {  namespace proto
{
    struct MessageBuilder
    {
        static inline void setSerial(Message& msg, const uint8_t* serial)
        {
            memcpy(&msg.serial, serial, SERIAL_LEN);
        }

        static inline void setToken(Message& msg, uint8_t* token)
        {
            memcpy(&msg.token, token, TOKEN_LEN);
        }

        static inline void setError(Message& msg, ErrorType error, const char* cause)
        {
            msg.type = MessageType::Error;
            msg.payload.error.e = error;
            memcpy(msg.payload.error.msg, cause, strlen(cause));
            msg.payload.error.msg[strlen(cause)] = '\0';
            msg.payloadLength = sizeof(msg.payload.error);
        }
        static Message handshake(const uint8_t* serial, byte_t api_release, byte_t api_major, byte_t api_minor, uint8_t* token)
        {
            Message msg;
            msg.type = MessageType::Handshake;

            setSerial(msg, serial);
            setToken(msg, token);
         
            msg.payload.handshake.maximumVersion.Release = api_release;
            msg.payload.handshake.maximumVersion.Major = api_major;
            msg.payload.handshake.maximumVersion.Minor = api_minor;

            msg.payload.handshake.minimumVersion.Release = api_release;
            msg.payload.handshake.minimumVersion.Major = api_major;
            msg.payload.handshake.minimumVersion.Minor = api_minor;


            msg.payload.handshake.desiredVersion.Release = api_release;
            msg.payload.handshake.desiredVersion.Major = api_major;
            msg.payload.handshake.desiredVersion.Minor = api_minor;

            msg.payloadLength = sizeof(msg.payload.command);

            return msg;
        }
    };
} }

#endif // AB_PROTO_MESSAGE_BUILDER_H