#ifndef AB_PROTO_HANDSHAKE_PAYLOAD_H
#define AB_PROTO_HANDSHAKE_PAYLOAD_H

#include "types.h"

namespace aquabox
{
    namespace proto
    {
        enum class HandshakeResult : byte_t
        {
            Ok = 0,
            RetryLater = 1,
            Fail = 255
        };

        struct ApiVersion
        {
            uint8_t Release;
            uint8_t Major;
            uint8_t Minor;
        } __attribute__((packed));

        struct HandshakePayload
        {
            ApiVersion desiredVersion;
            ApiVersion minimumVersion;
            ApiVersion maximumVersion;
            HandshakeResult result;
        } __attribute__((packed));

    } // namespace proto
} // namespace aquabox

#endif // AB_PROTO_ERROR_PAYLOAD_H