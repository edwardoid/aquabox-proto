#ifndef AB_PROTO_TYPES_H
#define AB_PROTO_TYPES_H

#include <stdint.h>


#define TOKEN_LEN 8
#define SERIAL_LEN 8

#ifdef NO_VIRTUAL_FN
    #define VIRTUAL_FN
    #define PURE() {}
    #define PURE_T(Type) { return T() }
#else
    #define VIRTUAL_FN virtual
    #define PURE = 0
    #define PURE_T(Type) = 0
#endif

namespace aquabox
{
    namespace proto
    {
        using byte_t = uint8_t;
        using buffer_length_t = uint16_t;
    }
}

#endif // AB_PROTO_TYPES_H