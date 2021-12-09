#ifndef AB_PROTO_SLAVE_H
#define AB_PROTO_SLAVE_H

#include "io.h"
#include "message_builder.h"

namespace aquabox {
namespace proto {
    class Slave {
    public:
        Slave(IO* io, const byte_t* serial, const byte_t* token);

        bool handshake();

        VIRTUAL_FN void loop();
        VIRTUAL_FN uint8_t propertiesCount() const PURE_T(uint8_t);
        VIRTUAL_FN const char* propertyName(uint8_t index) const PURE_T(const char*);
        VIRTUAL_FN int8_t propertyIndex(const char*) const PURE_T(uint8_t);
        VIRTUAL_FN ValueType propertyType(uint8_t index) const PURE_T(ValueType);
        VIRTUAL_FN bool set(uint8_t property, const ValueData& value) PURE_T(bool);
        VIRTUAL_FN bool get(uint8_t property, ValueData& value) PURE_T(bool);
        VIRTUAL_FN bool processNextMessage();

    protected:
        bool dispatch(Message* message, Message* response);
        bool handleCommandRequest(Message* msg, Message* response);

    protected:
        IO* m_io;
        const byte_t m_serial[SERIAL_LEN];
        byte_t m_token[TOKEN_LEN];
    };

} // namespace proto
} // namespace aquabox

#endif // RELAY_SLAVE_H
