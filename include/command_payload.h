#ifndef AB_PROTO_COMMAND_PAYLOAD_H
#define AB_PROTO_COMMAND_PAYLOAD_H

#include "command_data.h"

namespace aquabox
{
    namespace proto
    {
        enum class Command: byte_t
        {
            Set = 2,
            Get = 3,
            GetPropertiesCount = 4,
            GetPropertyName  = 5
        };

        struct CommandPayload
        {
            Command command;
            CommandData data;
        };
    } // namespace proto
} // namespace aquabox

#endif // AB_PROTO_COMMAND_PAYLOAD_H