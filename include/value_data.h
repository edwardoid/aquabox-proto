#ifndef AB_PROTO_VALUE_DATA_H
#define AB_PROTO_VALUE_DATA_H

#include "types.h"

namespace aquabox {
namespace proto {
    enum ValueType : uint8_t {
        Boolean = 0,
        Integer = 1,
        UnsignedInteger = 2,
        String = 3,
        Float = 4
    };

    struct FloatValue {
        int32_t V;
        uint16_t Precision;
    } __attribute__((packed));

    struct ValueData {
        char name[12]; // 12
        ValueType type; // 1
        union {
            uint8_t B; // 1
            int32_t I; // 4
            uint32_t U; // 4
            char S[64]; // 64
            FloatValue F;
        } value;
    } __attribute__((packed));

} // namespace proto
} // namespace aquabox

#endif // AB_PROTO_VALUE_DATA_H