#ifndef  AB_PROTO_ERROR_PAYLOAD_H
#define AB_PROTO_ERROR_PAYLOAD_H

#include "types.h"

namespace aquabox
{
    namespace proto
    {
        enum class ErrorType : uint8_t
        {
            OK = 0,
            Unsupported = 1,
            AccessDenied = 2,
            BadType = 3
        };

        struct ErrorPayload
        {
            ErrorType e;
            char msg[100];
        } __attribute__((packed));
    } // namespace proto
} // namespace aquabox

#endif // AB_PROTO_ERROR_PAYLOAD_H