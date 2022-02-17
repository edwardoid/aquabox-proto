#ifndef EASY_SLAVE_H
#define EASY_SLAVE_H

#include "easy_slave_property.h"
#include "slave.h"

namespace aquabox
{
    namespace proto
    {
        template<uint8_t PropertiesCount>
        class EasySlave : public Slave
        {
        public:
            using SlavePropertyPtr = SlaveProperty*;
            EasySlave(SlavePropertyPtr* props, IO* io, const byte_t* serial, const byte_t* token)
                : Slave(io, serial, token)
                , m_vars(props)
            {}

            VIRTUAL_FN uint8_t propertiesCount() const { return PropertiesCount; }
            VIRTUAL_FN const char* propertyName(uint8_t index) const { return m_vars[index]->name; }
            VIRTUAL_FN int8_t propertyIndex(const char* name) const
            {
                for (int i = 0; i < PropertiesCount; ++i) {
                    if (strcmp(m_vars[i]->name, name) == 0) {
                        return i;
                    }
                }

                return -1;
            }
            VIRTUAL_FN ValueType propertyType(uint8_t index) const { return m_vars[index]->type(); }
            VIRTUAL_FN bool set(uint8_t property, const ValueData& value) { return m_vars[property]->set(value); }
            VIRTUAL_FN bool get(uint8_t property, ValueData& value) { return m_vars[property]->get(value); }

        protected:
            void setProps(SlavePropertyPtr* props) { m_vars = props; }

        protected:
            SlavePropertyPtr* m_vars;
        };
    } // namespace proto
} // namespace aquabox

#endif // EASY_SLAVE_H
