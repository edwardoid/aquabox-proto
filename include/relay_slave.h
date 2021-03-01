#ifndef AB_PROTO_RELAY_SLAVE_H
#define AB_PROTO_RELAY_SLAVE_H

#include "slave.h"

namespace aquabox
{
    namespace proto
    {
        class RelaySlave: public Slave
        {
        public:
            RelaySlave(IO *io, const byte_t* serial, const byte_t* token)
                : Slave(io, serial, token)
            {
                setRelay(false);
            }

            VIRTUAL_FN bool setRelay(bool on)
            {
                m_on = on;
                return true;
            }
            
            VIRTUAL_FN inline uint8_t propertiesCount() const { return 2; };
            VIRTUAL_FN const char* propertyName(uint8_t index) const
            {
                if (index == 0) return DEVICE_TYPE;
                if (index == 1) return IS_ON;

                return NULL;
            }

            VIRTUAL_FN int8_t propertyIndex(const char* name) const
            {
                if (strncmp(name, DEVICE_TYPE, DEVICE_TYPE_LEN)) { return 0; }
                if (strncmp(name, IS_ON, IS_ON_LEN)) { return 1; }
                return -1;
            }

            VIRTUAL_FN ValueType propertyType(uint8_t index) const
            {
                if (index == 0) return ValueType::String;
                if (index == 1) return ValueType::Boolean;

                return ValueType();
            }
            VIRTUAL_FN bool set(uint8_t property, const ValueData& value)
            {
                if (property == 1) return setRelay(value.value.B);
                return false;
            }
            VIRTUAL_FN bool get(uint8_t property, ValueData& value)
            {
                if (property == 0)
                {
                    strncpy(value.value.S, "relay", strlen("relay"));
                    return true;
                }

                if (property == 1)
                {
                    value.value.B = m_on;
                    return true;
                }

                return false;
            }

        private:
            static const char* IS_ON;
            static const int IS_ON_LEN;
            static const char* DEVICE_TYPE;
            static const int DEVICE_TYPE_LEN;
        protected:
            bool m_on;
        };

        const char* RelaySlave::IS_ON("on");
        const int RelaySlave::IS_ON_LEN(strlen("on"));

        const char* RelaySlave::DEVICE_TYPE("deviceType");
        const int RelaySlave::DEVICE_TYPE_LEN(strlen("deviceType"));

    } // namespace proto
} // namespace aquabox

#endif // RELAY_SLAVE_H
