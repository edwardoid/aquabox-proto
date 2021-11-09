#ifndef AB_PROTO_IO_H
#define AB_PROTO_IO_H

#include "types.h"

namespace aquabox {
namespace proto {
    class IO {
    public:
        VIRTUAL_FN bool good() const PURE_T(bool);
        VIRTUAL_FN buffer_length_t available() const PURE_T(buffer_length_t);
        VIRTUAL_FN buffer_length_t read(buffer_length_t sz, byte_t* buffer) PURE_T(buffer_length_t);
        template <typename T>
        inline bool get(T& obj)
        {
            uint16_t a = available();

            if (sizeof(obj) > a) {
                wait(sizeof(obj) - a);
            }
            return sizeof(T) == read(sizeof(obj), reinterpret_cast<byte_t*>(&obj));
        }
        VIRTUAL_FN buffer_length_t write(const byte_t* buffer, buffer_length_t sz) PURE_T(buffer_length_t);
        template <typename T>
        inline bool send(const T& obj)
        {
            return sizeof(T) == write(reinterpret_cast<const byte_t*>(&obj), sizeof(obj));
        }

        VIRTUAL_FN buffer_length_t wait(buffer_length_t length) const { return available(); }
        VIRTUAL_FN void close() { }
    };
}
}

#endif // AB_PROTO_IO_H