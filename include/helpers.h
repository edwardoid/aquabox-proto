#ifndef AB_PROTO_HELPERS_H
#define AB_PROTO_HELPERS_H

#include "types.h"

namespace aquabox
{
    namespace proto
    {
        struct Helpers
        {
            static const uint8_t TemporaryToken[TOKEN_LEN];
            static bool isTemporaryToken(const uint8_t* token)
            {
                return TemporaryToken[0] == token[0] && TemporaryToken[1] == token[1] && TemporaryToken[2] == token[2]
                       && TemporaryToken[3] == token[3] && TemporaryToken[4] == token[4] && TemporaryToken[5] == token[5]
                       && TemporaryToken[6] == token[6] && TemporaryToken[7] == token[7];
            }
        };

        const uint8_t Helpers::TemporaryToken[TOKEN_LEN] = {'T', 0xAD, 0x00, 0x00, 0xBE, 0xEF, 0x00, 0x00};
    } // namespace proto
} // namespace aquabox

#endif // AB_PROTO_HELPERS_H