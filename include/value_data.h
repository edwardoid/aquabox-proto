#ifndef AB_PROTO_VALUE_DATA_H
#define AB_PROTO_VALUE_DATA_H

#include "types.h"

namespace aquabox
{
    namespace proto
    {
        enum ValueType : uint8_t
        {
            Boolean = 0,
            Integer = 1,
            UnsignedInteger = 2,
            String = 3,
            Float = 4
        };

        struct FloatValue
        {
            int32_t V;
            uint16_t Precision;
        };

        struct ValueData
        {
            char name[16];
            ValueType type;
            union
            {
                uint8_t B; // False = 0, True != 0
                int32_t I;
                uint32_t U;
                char S[64];
                FloatValue F;
            } value;
        };

    } // namespace proto
} // namespace aquabox

#endif // AB_PROTO_VALUE_DATA_H